from aiohttp import web
import uuid
import json

# status: 'waiting' -> 'preparing' -> 'success' -> 'playing'
# 房间已创建好，等待加入 -> 有玩家加入房间，准备ws连接 -> ws连接成功，进入游戏 -> 双方已准备，游戏进行

rooms = {}      # 存储所有房间信息
'''
房间信息结构：
{
    'host_ws': 房主WebSocket连接,
    'host_ip': 房主IP,
    'guest_ws': 玩家WebSocket连接,
    'guest_ip': 玩家IP,
    'status': 房间状态
}
'''

async def create_room(request):
    room_id = str(uuid.uuid4())[:6]     # 生成6位随机房间号
    ip = request.remote
    rooms[room_id] = {"host_ws": None, 'host_ip': ip, 'guest_ws': None, 'guest_ip': None, 'status': 'waiting'}
    return web.json_response({'roomId': room_id, 'status': 'waiting'})

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

async def webSocket_handler(request):
    ws = web.WebSocketResponse()
    await ws.prepare(request)                   # 准备WebSocket连接
    room_id = None
    async for msg in ws:                        # 监听WebSocket消息
        if msg.data.startswith('join:'):        # 该分支处理客户端加入房间
            room_id = msg.data.split(':')[1]    # 这里实现房间号获取
            room = rooms.get(room_id)
            if room:
                if room['host_ws'] is None:
                    room['host_ws'] = ws                    
                elif room['guest_ws'] is None:                    
                    room['guest_ws'] = ws
                    room['status'] = 'success'  # 双方已成功加入房间，可以开始游戏
                    # 发送成功消息给双方，代表可以进入联机场景
                    await room['host_ws'].send_str('success')
                    await room['guest_ws'].send_str('success')
        elif msg.data.startswith('move:') and room_id:  # 该分支处理客户端落子信息
            room = rooms.get(room_id)
            if room:
                # 落子消息的接收方为发送方的对手
                target = room['guest_ws'] if ws == room['host_ws'] else room['host_ws']
                if target:
                    await target.send_str(msg.data)
    # 一旦房主断开ws连接，或ws连接失败，销毁房间，即房间属于房主
    if room_id and room_id in rooms:
        if rooms[room_id]['host_ws'] == ws:
            del rooms[room_id]
        elif rooms[room_id]['host_ip'] == request.remote:
            del rooms[room_id]
    return ws

# 创建web应用实例，并添加路由处理函数
app = web.Application()
app.router.add_post('/create_room', create_room)
app.router.add_post('/join_room', join_room)
app.router.add_get('/ws', webSocket_handler)

# 启动服务器
web.run_app(app, host='0.0.0.0', port=8080)