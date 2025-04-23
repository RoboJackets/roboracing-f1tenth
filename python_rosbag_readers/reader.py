from rosbag2_py import Player, SequentialReader
from builtin_interfaces.msg import Time

reader = SequentialReader()
reader.open("~/bagfiles/test-ros-bag-2025-04-2019-04-25_911720/", {})
desired_start_time = 1000 * 10**9
reader.seek(desired_start_time)
player = Player(reader)
player.play()
