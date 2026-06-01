import os

class File:
    def __init__(self, name, size, offset, contents):
        self.name = name
        self.size = size
        self.offset = offset 
        self.contents = contents 

class Header:
    def __init__(self, version, node_count, root_node, nodes_offset):
        self.magic = 0x52445449
        self.version = version
        self.node_count = node_count
        self.root_node = root_node
        self.nodes_offset = nodes_offset

    def serialize(self):
        return (
            self.magic.to_bytes(4, "little") +
            self.version.to_bytes(4, "little") +
            self.node_count.to_bytes(4, "little") +
            self.root_node.to_bytes(4, "little") +
            self.nodes_offset.to_bytes(4, "little")
        )

class Node:
    def __init__(self, id, type, parent_id, name, size, data_offset):
        self.id = id
        self.type = type
        self.parent_id = parent_id
        self.name = name
        self.size = size
        self.data_offset = data_offset
    
    def serialize(self):
        name_bytes = self.name.encode("utf-8")
        name_bytes = name_bytes[:64]
        name_bytes = name_bytes.ljust(64, b"\x00")

        return (
            self.id.to_bytes(4, "little") +
            self.type.to_bytes(4, "little") +
            self.parent_id.to_bytes(4, "little", signed=False) +
            name_bytes +
            self.size.to_bytes(4, "little") +
            self.data_offset.to_bytes(4, "little")
        )


#globals for use inside the parsing functions
num_files = 0
nodes_offset = 20 # sizeof(initrd_superblock_t)
node_size = 84 # sizeof(initrd_node_t)
data_offset = nodes_offset + num_files * node_size 
nodes = []
file_info = []
file_nodes = []

def parse_file(file_name, parent_id):
    global num_files
    global file_info
    global nodes
    global data_offset

    with open(file_name, "rb") as file: 
        content = file.read()
        
        file_info.append(File(file_name, len(content), None, content))
        
        node = Node(num_files, 1, parent_id, file_name, len(content), None)
        nodes.append(node)
        file_nodes.append(node)
        num_files += 1

def parse_directory(dir_name, parent_id):
    global num_files
    global nodes
    
    old_dir = os.getcwd()

    os.chdir(dir_name)
    files = sorted(os.listdir("."))
    file_info = []
    id = num_files
    num_files += 1
    
    # since we must change to the fs folder we dont want that as the name of our root dir
    if id == 0:
        dir_name = "initrd"

    nodes.append(Node(id, 2, parent_id, dir_name, 0, 0))

    # get all file info
    for idx in range(len(files)):
        file_name = files[idx]
        if os.path.isdir(file_name):
            parse_directory(file_name, id)
        else:
            parse_file(file_name, id)
         
    
    os.chdir(old_dir)

# creates the nodes and files classses
parse_directory("fs", 0xFFFFFFFF)

# set offsets for each file
data_offset = nodes_offset + len(nodes) * node_size
current_offset = data_offset

for file_obj, node_obj in zip(file_info, file_nodes):
    file_obj.offset = current_offset
    node_obj.data_offset = current_offset
    current_offset += file_obj.size

# create header
header = Header(1, num_files, 0, nodes_offset)

image = bytearray()
image += header.serialize()

for node in nodes:
    image += node.serialize()

for file in file_info:
    image += file.contents

assert len(header.serialize()) == 20
assert all(len(node.serialize()) == 84 for node in nodes)

with open("initrd.img", "wb") as file:
    file.write(image)
