import numpy as np

class Robot:
    def __init__(self):
        self.position = np.array([[0], [0], [1]], dtype = np.float32)
        self.theta = 0
        self.x_dim = 1000
        self.y_dim = 1000
        # map is indexed x,y
        self.map = np.zeros(( self.x_dim, self.y_dim), dtype=np.float32)
        return

    def odom_update(self, r, theta):
        # x is forward, y is to the right
        dtheta = self.theta - theta
        transform_matrix = self.transform_matrix(r, dtheta)
        self.update_map(transform_matrix)
        self.position = transform_matrix @ self.position
        self.theta = theta

    def rotation_matrix(self, theta):
        return np.array([[np.cos(theta), -np.sin(theta),0], [np.sin(theta), np.cos(theta),0], [0,0,1]],
                        dtype = np.float32)

    def translate_matrix(self, r):
        return np.array([[1,0,r], [0,1,0], [0,0,1]],
                        dtype = np.float32)

    def transform_matrix(self, r, theta):
        return self.translate_matrix(r) @ self.rotation_matrix(theta)

    def update_map(self, transform_matrix):
        new_map = np.zeros(( self.x_dim, self.y_dim), dtype=np.float32)
        offset = np.array([[self.x_dim//2], [self.y_dim//2], [1]], dtype=np.float32)

        for x in range(self.x_dim):
            for y in range(self.y_dim):
                old_pos = np.array([[x],[y],[0]], dtype=np.float32)
                new_pos = (transform_matrix @ (old_pos-offset)) + offset
                new_map[x,y] = self.sample_map(new_pos)
        self.map = new_map

    def sample_map(self, position):
        c1 = np.array([np.floor(position[0,0]), np.floor(position[1,0])], dtype = np.int32)
        c2 = np.array([np.floor(position[0,0]), np.ceil(position[1,0])], dtype=np.int32)
        c3 = np.array([np.ceil(position[0,0]), np.floor(position[1,0])], dtype=np.int32)
        c4 = np.array([np.ceil(position[0,0]), np.ceil(position[1,0])], dtype=np.int32)

        x_percent = position[0] - np.floor(position[0])
        y_percent = position[1] - np.floor(position[1])

        v1,v2,v3,v4 = 0,0,0,0

        if 0 <= c1[0] < self.x_dim and 0 <= c1[1] < self.y_dim:
            v1 = x_percent * y_percent * self.map[tuple(c1)]
            v1 = v1[0]
        if 0 <= c2[0] < self.x_dim and 0 <= c2[1] < self.y_dim:
            v2 = x_percent * (1-y_percent) * self.map[tuple(c2)]
            v2 = v2[0]
        if 0 <= c3[0] < self.x_dim and 0 <= c3[1] < self.y_dim:
            v3 = (1-x_percent) * y_percent * self.map[tuple(c3)]
            v3 = v3[0]
        if 0 <= c4[0] < self.x_dim and 0 <= c4[1] < self.y_dim:
            v4 = (1-x_percent) * (1-y_percent) * self.map[tuple(c4)]
            v4 = v4[0]

        return v1+v2+v3+v4

def main():
    robot = Robot()
    robot.map[:,4] = 1
    # print(robot.map)
    for i in range(10**6):
        robot.sample_map(np.array([[1],[1]]))
    # robot.odom_update(2.3, np.pi/4)
    # print(np.round(robot.map, 1))

if __name__ == "__main__":
    main()