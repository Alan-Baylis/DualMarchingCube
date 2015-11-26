import numpy as np
import matplotlib.pyplot as plt
# http://stackoverflow.com/questions/471962/how-do-determine-if-a-polygon-is-complex-convex-nonconvex
def is_convex(pts):
    N = pts.shape[0]
    sign = None

    stack = []
    for i in xrange(N):
        dx1 = pts[(i+2)%N,0] - pts[(i+1)%N,0]
        dy1 = pts[(i+2)%N,1] - pts[(i+1)%N,1]
        dx2 = pts[i,0] - pts[(i+1)%N,0]
        dy2 = pts[i,1] - pts[(i+1)%N,1]

        cross_z = dx1 * dy2 - dx2 * dy1
        stack.append(cross_z)
        if sign is None:
            sign = [-1, 1][cross_z > 0]
        elif sign * cross_z < 0:
            print i, stack
            return False
    return True

if __name__ == '__main__':
    pts = np.random.rand(4,2)
    '''
    pts = [[ 0.84987003, 0.18621169],   \
            [ 0.60694906, 0.78514431],  \
            [ 0.30490175, 0.78103365],  \
            [ 0.35901865, 0.23070076]]
    pts = np.array(pts)
    '''
    print pts
    print is_convex(pts)

    for i in xrange(pts.shape[0]):
        plt.annotate(str(i), xy = pts[i,:])

    pts_plot = np.zeros((5,2))
    pts_plot[:4,:] = pts
    pts_plot[4,:] = pts[0,:]
    plt.plot(pts_plot[:,0], pts_plot[:,1])
    plt.show()
