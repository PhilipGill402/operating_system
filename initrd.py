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


# look for files in fs/
os.chdir("fs/")
files = sorted(os.listdir("."))
print(files)
file_info = []

num_files = len(files) + 1 # add 1 for 'root'
nodes_offset = 20 # sizeof(initrd_superblock_t)
node_size = 84 # sizeof(initrd_node_t)
data_offset = nodes_offset + num_files * node_size 

# get all file info
for idx in range(len(files)):
    file_name = files[idx]
    with open(file_name, "rb") as file: 
        content = file.read()
        
        if idx == 0:
            offset = data_offset
        else:
            offset = file_info[idx - 1].offset + file_info[idx - 1].size

        file_info.append(File(file_name, len(content), offset, content))

# create nodes
nodes = [Node(0, 2, 0xFFFFFFFF, "/", 0, 0)] # intialize the list with the root node

node_id = 1
for file in file_info:
    nodes.append(Node(node_id, 1, 0, file.name, file.size, file.offset))
    node_id += 1

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

os.chdir("..")
with open("initrd.img", "wb") as file:
    file.write(image)
