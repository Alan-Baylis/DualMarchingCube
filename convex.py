import numpy as np
import matplotlib.pyplot as plt
# http://stackoverflow.com/questions/471962/how-do-determine-if-a-polygon-is-complex-convex-nonconvex
def is_convex(pts):
    N = pts.shape[0]
    sign = None

    stack = []
    for i in xrange(N):
        dx1 = pts[(i+N-1)%N,0] - pts[i,0]
        dy1 = pts[(i+N-1)%N,1] - pts[i,1]
        dx2 = pts[(i+1)%N,0] - pts[i,0]
        dy2 = pts[(i+1)%N,1] - pts[i,1]

        cross_z = dx1 * dy2 - dx2 * dy1
        stack.append(cross_z > 0)
        '''
        if sign is None:
            sign = [-1, 1][cross_z > 0]
        elif sign * cross_z < 0:
            
            return False
        '''
    print stack
    return len(set(stack)) == 1
    # return True

if __name__ == '__main__':
    # pts = np.random.rand(4,2)
    
    pts = [ [-1.15461111, -1.15461111],   \
            [-0.923076868, -1.2339797],  \
            [-0.924950122, -0.924950122],  \
            [-1.2339797, -0.923076868]]
    pts = np.array(pts)
    
    print '{'
    for i in xrange(4):
        print '   {%f, %f},' % (pts[i,0], pts[i,1])
    print '};'
    print is_convex(pts)

    for i in xrange(pts.shape[0]):
        plt.annotate(str(i), xy = pts[i,:])

    plt.annotate('origin', xy = (-1.15384603, -1.15384603))
    pts_plot = np.zeros((5,2))
    pts_plot[:4,:] = pts
    pts_plot[4,:] = pts[0,:]
    plt.plot(pts_plot[:,0], pts_plot[:,1])
    plt.show()
