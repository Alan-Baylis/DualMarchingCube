from copy import deepcopy
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt

pt_rot_x_map = {0:3, 3:7, 7:4, 4:0, 1:2, 2:6, 6:5, 5:1}
pt_rot_y_map = {0:4, 4:5, 5:1, 1:0, 3:7, 7:6, 6:2, 2:3}
pt_rot_z_map = {0:1, 1:2, 2:3, 3:0, 4:5, 5:6, 6:7, 7:4}

pt_pair_edge_map = {(0,1):0, (1,2):1, (2,3):2,  (0,3):3, \
                    (0,4):4, (1,5):5, (2,6):6,  (3,7):7, \
                    (4,5):8, (5,6):9, (6,7):10, (4,7):11}
edge_pt_pair_map = {v:k for k, v in pt_pair_edge_map.iteritems()}

NO_VERTEX = 0xff
VOXEL_NUM_PTS = 8
VOXEL_NUM_EDGES = 12

def encode_voxel_config(vx_config):
    vx_index = 0x00
    for i in xrange(len(vx_config)):
        vx_index |= (vx_config[i] << i)
    return vx_index

def decode_voxel_config(vx_index):
    dim = 8
    result = [0] * dim
    for i in xrange(dim):
        result[i] = vx_index & 0x01
        vx_index >>= 1
    return result

def rotate_to_new_table_entry(old_vx_index, old_entry, pt_rotate_map):
    old_vx_config_lst = decode_voxel_config(old_vx_index)
    pts_dim = len(old_vx_config_lst)
    new_vx_config_lst = [0] * pts_dim
    for pt_index in xrange(pts_dim):
        new_vx_config_lst[pt_rotate_map[pt_index]] = old_vx_config_lst[pt_index]
    new_vx_index = encode_voxel_config(new_vx_config_lst)

    len_entry = len(old_entry)
    new_entry = [NO_VERTEX] * len_entry
    for edge_index in xrange(len_entry):
        pt_pair = edge_pt_pair_map[edge_index]
        old_p0, old_p1 = pt_pair
        new_p0, new_p1 = pt_rotate_map[old_p0], pt_rotate_map[old_p1]
        if new_p0 > new_p1:
            new_p0, new_p1 = new_p1, new_p0
        mapped_edge_index = pt_pair_edge_map[(new_p0, new_p1)]
        new_entry[mapped_edge_index] = old_entry[edge_index]
    return new_vx_index, new_entry

lv1_instr = [(0,'.'), (1,'X'),(2,'X'),(3,'X'),(1,'Y'),(3,'Y')]
lv2_instr_map = {\
    (0,'.'):['Z','Z','Z'],
    (1,'X'):['Y','Y','Y'],
    (2,'X'):['Z','Z','Z'],
    (3,'X'):['Y','Y','Y'],
    (1,'Y'):['X','X','X'],
    (3,'Y'):['X','X','X']}

def get_pt_rotate_map(rot_axis):
    if rot_axis == 'X':
        return pt_rot_x_map
    elif rot_axis == 'Y':
        return pt_rot_y_map
    elif rot_axis == 'Z':
        return pt_rot_z_map
    assert False

def exec_lv1_instr(instr, vx_index, entry):
    rot_times, rot_axis = instr
    if rot_times == 0:
        return vx_index, entry

    pt_rotate_map = get_pt_rotate_map(rot_axis)
    for i in xrange(rot_times):
        vx_index, entry = rotate_to_new_table_entry(vx_index, entry, pt_rotate_map)
    return vx_index, entry

def generate_single(vx_index, entry):
    result = {}
    instr_stack = []
    instr_record = {}
    def add_to_result(k, v):
        if k in result:
            v_existed = result[k]
            vertex_map = {}
            try:
                for i in xrange(len(v)):
                    if v[i] == NO_VERTEX:
                        assert v_existed[i] == v[i]
                    elif v[i] not in vertex_map:
                        vertex_map[v[i]] = v_existed[i]
                    else:
                        assert vertex_map[v[i]] == v_existed[i]
            except:
                # print (bin(vx_index), bin(k), v, result[k], instr_stack, instr_record[k])
                assert False
        else:
            result[k] = v
            # instr_record[k] = tuple(instr_stack)

    for instr in lv1_instr:
        # instr_stack.append(instr)
        new_vx_index, new_entry = exec_lv1_instr(instr, vx_index, entry)
        
        # print instr_stack, bin(new_vx_index), new_entry
        add_to_result(new_vx_index, new_entry)
        
        for lv2_instr in lv2_instr_map[instr]:
            # instr_stack.append(lv2_instr)
            pt_rotate_map = get_pt_rotate_map(lv2_instr)
            new_vx_index, new_entry = rotate_to_new_table_entry(new_vx_index, new_entry, pt_rotate_map)

            # print instr_stack, bin(new_vx_index), new_entry
            add_to_result(new_vx_index, new_entry)

        # instr_stack = []
    return result

def generate_all(init_table):
    result = deepcopy(init_table)
    for vx_index, entry in init_table.iteritems():
        cur_result = generate_single(vx_index, entry)

        old_len = len(result)
        result.update(cur_result)
        # assert len(result) == old_len + len(cur_result)
    return result

def play_around():
    vx_index = 0x1
    vx_config = decode_voxel_config(vx_index)
    print 'vx_index: ', bin(vx_index)
    print 'decoded: ', vx_config
    vx_index = encode_voxel_config(vx_config)
    print 'encoded again: ', bin(vx_index)

    entry = [NO_VERTEX] * 12
    entry[0], entry[3], entry[4] = 0, 0, 0
    rot_vx_index, new_entry = rotate_to_new_table_entry(vx_index, entry, pt_rot_z_map)
    print 'vx_index after rotate z: ',  decode_voxel_config(rot_vx_index)
    print 'old entry: ', entry
    print 'new entry: ', new_entry

def generate_init_table(use_lut1=True):
    init_table = {}

    def init_vx_index(pts):
        vx_config = [0] * VOXEL_NUM_PTS
        for p in pts:
            vx_config[p] = 1
        vx_index = encode_voxel_config(vx_config)
        return vx_index

    def init_entry():
        return [NO_VERTEX] * VOXEL_NUM_EDGES

    def set_entry(entry, edges, vertex):
        for e in edges:
            entry[e] = vertex
        return entry
    
    # 1
    vx_index = init_vx_index([5])
    entry = init_entry()
    entry = set_entry(entry, [5,8,9], 0)
    init_table[vx_index] = entry
    # 2A    
    vx_index = init_vx_index([6,7])
    entry = init_entry()
    entry = set_entry(entry, [6,7,9,11], 0)
    init_table[vx_index] = entry
    if use_lut1:
        # 2B-1
        vx_index = init_vx_index([4,6])
        entry = init_entry()
        entry = set_entry(entry, [4,6,8,9,10,11], 0)
        init_table[vx_index] = entry
    else:
        # 2B-2
        vx_index = init_vx_index([4,6])
        entry = init_entry()
        entry = set_entry(entry, [6,9,10], 0)
        entry = set_entry(entry, [4,8,11], 1)
        init_table[vx_index] = entry
    # 2C
    vx_index = init_vx_index([1,7])
    entry = init_entry()
    entry = set_entry(entry, [0,1,5], 0)
    entry = set_entry(entry, [7,10,11], 1)
    init_table[vx_index] = entry
    # 3A
    vx_index = init_vx_index([4,5,7])
    entry = init_entry()
    entry = set_entry(entry, [4,5,7,9,10], 0)
    init_table[vx_index] = entry
    if use_lut1:
        # 3B-1
        vx_index = init_vx_index([2,4,6])
        entry = init_entry()
        entry = set_entry(entry, [1,2,4,8,9,10,11], 0)
        init_table[vx_index] = entry
    else:
        # 3B-2
        vx_index = init_vx_index([2,4,6])
        entry = init_entry()
        entry = set_entry(entry, [1,2,9,10], 0)
        entry = set_entry(entry, [4,8,11], 1)
        init_table[vx_index] = entry
    # 3C
    vx_index = init_vx_index([2,5,7])
    entry = init_entry()
    entry = set_entry(entry, [1,2,5,7,8,11], 0)
    entry = set_entry(entry, [6,9,10], 1)
    init_table[vx_index] = entry
    # 4A
    vx_index = init_vx_index([4,5,6,7])
    entry = init_entry()
    entry = set_entry(entry, [4,5,6,7], 0)
    init_table[vx_index] = entry
    # 4B
    vx_index = init_vx_index([2,5,6,7])
    entry = init_entry()
    entry = set_entry(entry, [1,2,5,7,8,11], 0)
    init_table[vx_index] = entry
    # 4C
    vx_index = init_vx_index([1,3,5,7])
    entry = init_entry()
    entry = set_entry(entry, [0,3,8,11], 0)
    entry = set_entry(entry, [1,2,9,10], 1)
    init_table[vx_index] = entry
    # 4D-1 mirror
    vx_index = init_vx_index([1,2,6,7])
    entry = init_entry()
    entry = set_entry(entry, [0,2,5,7,9,11], 0)
    init_table[vx_index] = entry
    # 4D-2 mirror
    vx_index = init_vx_index([0,1,2,6])
    entry = init_entry()
    entry = set_entry(entry, [2,3,4,5,9,10], 0)
    init_table[vx_index] = entry
    # 4E
    vx_index = init_vx_index([0,1,2,7])
    entry = init_entry()
    entry = set_entry(entry, [2,3,7], 0)
    entry = set_entry(entry, [4,5,6,10,11], 1)
    init_table[vx_index] = entry
    # 4F
    vx_index = init_vx_index([1,3,4,6])
    entry = init_entry()
    entry = set_entry(entry, [0,3,4], 0)
    entry = set_entry(entry, [1,2,6], 1)
    entry = set_entry(entry, [5,8,9], 2)
    entry = set_entry(entry, [7,10,11], 3)
    init_table[vx_index] = entry
    # 5A
    vx_index = init_vx_index([0,1,2,3,6])
    entry = init_entry()
    entry = set_entry(entry, [4,5,7,9,10], 0)
    init_table[vx_index] = entry
    # 5B
    vx_index = init_vx_index([0,1,3,5,7])
    entry = init_entry()
    entry = set_entry(entry, [1,2,9,10], 0)
    entry = set_entry(entry, [4,8,11], 1)
    init_table[vx_index] = entry
    # 5C
    vx_index = init_vx_index([0,1,3,4,6])
    entry = init_entry()
    entry = set_entry(entry, [1,2,6], 0)
    entry = set_entry(entry, [5,8,9], 1)
    entry = set_entry(entry, [7,10,11], 2)
    init_table[vx_index] = entry
    # 6A
    vx_index = init_vx_index([0,1,2,3,4,5])
    entry = init_entry()
    entry = set_entry(entry, [6,7,9,11], 0)
    init_table[vx_index] = entry
    # 6B
    vx_index = init_vx_index([0,1,2,3,5,7])
    entry = init_entry()
    entry = set_entry(entry, [6,9,10], 0)
    entry = set_entry(entry, [4,8,11], 1)
    init_table[vx_index] = entry
    # 6C
    vx_index = init_vx_index([0,2,3,4,5,6])
    entry = init_entry()
    entry = set_entry(entry, [0,1,5], 0)
    entry = set_entry(entry, [7,10,11], 1)
    init_table[vx_index] = entry
    # 7
    vx_index = init_vx_index([0,1,2,3,4,6,7])
    entry = init_entry()
    entry = set_entry(entry, [5,8,9], 0)
    init_table[vx_index] = entry
    '''
    vx_index = init_vx_index([])
    entry = init_entry()
    entry = set_entry(entry, [], )
    init_table[vx_index] = entry
    '''
    return init_table

def generate_2B_3B_table():
    init_table = {}

    def init_vx_index(pts):
        vx_config = [0] * VOXEL_NUM_PTS
        for p in pts:
            vx_config[p] = 1
        vx_index = encode_voxel_config(vx_config)
        return vx_index

    def init_entry():
        return [NO_VERTEX] * VOXEL_NUM_EDGES

    def set_entry(entry, edges, vertex):
        for e in edges:
            entry[e] = vertex
        return entry
    
    # 2B-1
    '''
    vx_index = init_vx_index([4,6])
    entry = init_entry()
    entry = set_entry(entry, [4,6,8,9,10,11], 0)
    init_table[vx_index] = entry
    '''
    '''
    # 2B-2
    vx_index = init_vx_index([4,6])
    entry = init_entry()
    entry = set_entry(entry, [6,9,10], 0)
    entry = set_entry(entry, [4,8,11], 1)
    init_table[vx_index] = entry
    '''
    
    # 3B-1
    vx_index = init_vx_index([2,4,6])
    entry = init_entry()
    entry = set_entry(entry, [1,2,4,8,9,10,11], 0)
    init_table[vx_index] = entry
    
    '''
    # 3B-2
    vx_index = init_vx_index([2,4,6])
    entry = init_entry()
    entry = set_entry(entry, [1,2,9,10], 0)
    entry = set_entry(entry, [4,8,11], 1)
    init_table[vx_index] = entry
    '''
    return init_table

def draw_table_config(vx_index, lut):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    # plot cube
    pts = [(0,0,0), (1,0,0), (1,1,0), (0,1,0),(0,0,1), (1,0,1), (1,1,1), (0,1,1)]
    pts = np.array([np.array(c, dtype=np.float32) for c in pts])
    offset_constant = 0.05

    for i in xrange(pts.shape[0]):
        pts_offset = np.copy(pts[i, :])
        for j in xrange(3):
            if pts_offset[j] > 0:
                pts_offset[j] += offset_constant
            else:
                pts_offset[j] -= offset_constant
        ax.text(pts_offset[0], pts_offset[1], pts_offset[2], i) 

    edges = [(0,1), (1,2), (2,3), (0,3), (0,4), (1,5), (2,6), (3,7), (4,5),\
            (5,6), (6,7), (4,7)]
    local_edges = set([6, 9, 10])
    for i in xrange(len(edges)):
        frm_index, to_index = edges[i]
        xyz = np.zeros((2, 3))
        xyz[0,:] = pts[frm_index,:]
        xyz[1,:] = pts[to_index,:]

        colorstr = 'gray'
        if i in local_edges:
            colorstr = 'red'
        ax.plot(xyz[:,0], xyz[:,1], xyz[:,2], color=colorstr)

        center_x, center_y, center_z = 0.5 * (sum(xyz[:,0])), 0.5 * (sum(xyz[:,1])), 0.5 * (sum(xyz[:,2]))
        ax.text(center_x, center_y, center_z, i, color='#2300bc')

    faces = [(0,1,2,3),(0,1,4,5),(1,2,5,6),(2,3,6,7),(0,3,7,4),(4,5,6,7)]
    for i in xrange(len(faces)):
        center_xyz = np.zeros(3)
        for p in faces[i]:
            center_xyz += pts[p]
        center_xyz *= 0.25
        ax.text(center_xyz[0], center_xyz[1], center_xyz[2], i, color='#40ca23')

    vx_config = decode_voxel_config(vx_index)
    for i in xrange(len(vx_config)):
        if vx_config[i]:
            pt = pts[i]
            ax.scatter(pt[0], pt[1], pt[2], color='orange', s=100)

    entry = lut[vx_index]

    vertex_lst = [(.5,.5,.5), (.4,.4,.5), (.4,.5,.5), (.5,.4,.5)]
    vertex_lst = np.array([np.array(t) for t in vertex_lst])
    face_center = [(-1, -1, 0), (-1, 0, -1), (1., -1, -1), \
                    (-1, 1., -1), (0, -1, -1), (-1, -1, 1.)]
    face_center = np.array([np.array(t) for t in face_center])
    associate_faces_map = { \
        0: (0,1), 1:(0,2), 2:(0,3), 3:(0,4), \
        4: (1,4), 5:(1,2), 6:(2,3), 7:(3,4), \
        8: (1,5), 9:(2,5), 10:(3,5), 11:(4,5) }

    color_lst = [(0.9,0.1,0.1,0.2), (0.1,0.9,0.1,0.2), (0.1,0.1,0.9,0.2), (0.3,0.4,0.5,0.2)]
    for edge_index in xrange(len(entry)):
        if entry[edge_index] < NO_VERTEX:
            for face in associate_faces_map[edge_index]:
                xyz = np.zeros(6).reshape((2, 3))            
                xyz[0,:] = vertex_lst[entry[edge_index]]
                xyz[1,:] = vertex_lst[entry[edge_index]]
                for i in xrange(3):
                    if face_center[face,i] > -0.5:
                        xyz[1,i] = face_center[face,i]
                ax.plot(tuple(xyz[:, 0]), tuple(xyz[:, 1]), tuple(xyz[:, 2]), color=color_lst[entry[edge_index]])

    # plot coordinate framework
    xyz = np.zeros(6)
    xyz.fill(-0.25)
    xyz = xyz.reshape((2, 3))
    labels = ['x', 'y', 'z']
    for i in xrange(3):
        xyz[1, i] += 0.4
        ax.plot(tuple(xyz[:, 0]), tuple(xyz[:, 1]), tuple(xyz[:, 2]), color='b')
        center_x, center_y, center_z = 0.5 * (sum(xyz[:,0])), 0.5 * (sum(xyz[:,1])), 0.5 * (sum(xyz[:,2]))
        ax.text(center_x, center_y, center_z, labels[i])
        xyz[1,:] = xyz[0,:]

    plt.show()

def print_lut(lut):
    print '{'
    for k in xrange(len(lut)):
        entry = lut[k]
        entry = map(lambda x: ['0xff', str(x)][x < 0xff], entry)
        print '    { ' + ', '.join(entry) + ' }, '
    print '}'

def print_count_lut(lut):
    print '{'
    for k in xrange(0, len(lut), 16):
        counts = [0] * 16
        for j in xrange(16):
            counts[j] = str(len(set(lut[k + j]) - set([NO_VERTEX])))
        print '    ' + ', '.join(counts) + ', '
    print '}'

if __name__ == '__main__':
    
    # init_table = generate_init_table(use_lut1=False)
    init_table = generate_2B_3B_table()

    lut = generate_all(init_table)
    # lut[0x00] = [NO_VERTEX] * VOXEL_NUM_EDGES
    # lut[0xff] = [NO_VERTEX] * VOXEL_NUM_EDGES
    # for k in xrange(len(lut)):
    #    print decode_voxel_config(k), lut[k]
    '''
    keys = []
    for k in lut.keys():
        keys.append(format(k, '#04x'))
    print ', '.join(keys)
    '''
    print 'len: ', len(lut)
    # print_lut(lut)
    # print_count_lut(lut)
    
    count = 0
    for k in lut.keys():
        print count, format(k, '#04x')
        draw_table_config(k, lut)
        count += 1