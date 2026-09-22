'''
服务端负责处理：创建房间、响应客户端加入房间、确认客户端准备状态、转发落子信息、为客户端计算并推送回合时间（即服务器权威计时，避免
客户端计时不统一）、销毁房间
'''

from aiohttp import web
import uuid
import json
import asyncio

# status: 'waiting' -> 'preparing' -> 'success' -> 'playing'
# 房间状态：房间已创建好，等待加入 -> 有玩家加入房间，准备ws连接 -> ws连接成功，进入游戏 -> 双方已准备，游戏进行

rooms = {}      # 存储所有房间信息
'''
房间信息结构：
{
    'host_ws': 房主WebSocket连接,
    'host_ip': 房主IP,
    'host_ready': 房主准备状态,    
    'guest_ws': 玩家WebSocket连接,
    'guest_ip': 玩家IP,
    'guest_ready': 玩家准备状态, 
    'status': 房间状态
}
'''
timer_task = {} # 存储所有计时，每个房间有各自独立的计时

# 创建房间处理
async def create_room(request):
    room_id = str(uuid.uuid4())[:6]     # 生成6位随机房间号
    ip = request.remote
    rooms[room_id] = {'host_ws': None, 'host_ip': ip, 'host_ready': False, 'guest_ws': None, 'guest_ip': None, 'guest_ready': False, 'status': 'waiting'}
    return web.json_response({'roomId': room_id, 'status': 'waiting'})

# 加入房间处理
async def join_room(request):
    data = await request.json()         # 获取请求体中的JSON数据，只有房间号
    room_id = data.get('roomId')
    # 根据不同情况处理，加dumps是为了解决中文乱码问题，不加的话显示的是ASCII码
    if room_id not in rooms:
        return web.json_response({'status': 'error', 'msg': '房间不存在，请重新输入房间号'}, dumps=lambda x: json.dumps(x, ensure_ascii=False))
    elif rooms[room_id]['status'] != 'waiting':
        return web.json_response({'status': 'error', 'msg': '房间已满，请重新输入房间号'}, dumps=lambda x: json.dumps(x, ensure_ascii=False))
    ip = request.remote
    # 不允许同IP加入同一个房间
    if ip == rooms[room_id]['host_ip']:
        return web.json_response({'status': 'error', 'msg': '不能自娱自乐，请重新输入房间号'}, dumps=lambda x: json.dumps(x, ensure_ascii=False))
    rooms[room_id]['guest_ip'] = ip
    rooms[room_id]['status'] = 'preparing'
    return web.json_response({'roomId': room_id, 'status': 'preparing'})

# 开始回合计时处理
async def start_timer(room_id):
    # 一个回合时间为20秒，按1秒递减，然后向两个玩家推送剩余回合时间
    for roundSurplusTime in range(20, -1, -1):
        if room_id not in rooms:
            return
        await rooms[room_id]['host_ws'].send_str(f'update:{roundSurplusTime}')
        await rooms[room_id]['guest_ws'].send_str(f'update:{roundSurplusTime}')
        await asyncio.sleep(1)

# 重置回合处理
def reset_timer(room_id):
    if room_id in timer_task:
        timer_task[room_id].cancel()                                # 取消旧任务，但不删除记录
    timer_task[room_id] = asyncio.create_task(start_timer(room_id)) # 创建新任务

# 客户端与服务端建立 socket 连接处理
async def webSocket_handler(request):
    ws = web.WebSocketResponse()
    await ws.prepare(request)                   # 准备WebSocket连接
    room_id = None
    async for msg in ws:                        # 监听WebSocket消息
        # 该分支处理客户端加入房间
        if msg.data.startswith('join:'):
            room_id = msg.data.split(':')[1]    # 这里实现房间号获取
            room = rooms.get(room_id)           
            if room['host_ws'] is None:
                room['host_ws'] = ws            # 玩家创建房间时走到这一步
            elif room['guest_ws'] is None:
                room['guest_ws'] = ws           # 玩家加入房间时走到这一步
                room['status'] = 'success'      # 双方已成功加入房间，可以开始游戏
                # 发送成功消息给双方，代表可以进入联机场景
                await room['host_ws'].send_str('success')
                await ws.send_str('success')                   

        # 该分支处理双方准备，当双方都准备好才能开始游戏
        elif msg.data == 'ready:ok' and room_id:
            room = rooms.get(room_id)
            if ws == room['host_ws']:
                room['host_ready'] = True
            elif ws == room['guest_ws']:
                room['guest_ready'] = True
            if room['host_ready'] and room['guest_ready']:
                room['host_ready'] = False                          # 重置玩家准备状态，用于下一次游戏
                room['guest_ready'] = False
                room['status'] = 'playing'
                await room['host_ws'].send_str('role:black')        # 分配游戏角色
                await room['guest_ws'].send_str('role:white')
                await room['host_ws'].send_str('status:playing')    # 通知开始游戏
                await room['guest_ws'].send_str('status:playing')
                reset_timer(room_id)                                # 开始计时

        # 该分支处理客户端落子信息
        elif msg.data.startswith('move:') and room_id:
            room = rooms.get(room_id)           
            # 落子消息的接收方为发送方的对手
            target = room['guest_ws'] if ws == room['host_ws'] else room['host_ws']
            await target.send_str(msg.data)                         # 转发落子信息
            reset_timer(room_id)                                    # 更新计时

    if room_id and room_id in rooms:
        room = rooms.get(room_id)       # 提前拿到房间信息，就算rooms里的房间被删除了，这里还存在
        # 一旦房主断开ws连接，或ws连接失败，销毁房间，即房间属于房主
        if room['host_ws'] == ws:
            del rooms[room_id]
        elif room['host_ip'] == request.remote:
            del rooms[room_id]

        # 如果双方已进入游戏场景，但是还没开始游戏，有一方退出了，房间都得被销毁，并且双方都应该退出场景
        if room['status'] == 'success':
            other_ws = room['host_ws'] if ws == room['guest_ws'] else room['guest_ws']
            await other_ws.send_str('notice:opponentQuit')          # 告诉对手退出房间
            if room_id in rooms:        # 先检查房间是否存在，因为如果是房主退出房间，在前面分支里就会被删除了
                del rooms[room_id]      
    return ws

# 创建web应用实例，并添加路由处理函数
app = web.Application()
app.router.add_post('/create_room', create_room)
app.router.add_post('/join_room', join_room)
app.router.add_get('/ws', webSocket_handler)

# 启动服务器
web.run_app(app, host='0.0.0.0', port=8080)