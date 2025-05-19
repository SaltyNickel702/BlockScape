# Chunk Encoding
#### At start of chunk
First four bytes are x coord, next four are y
<br>
#### Next bytes are block data for each chunk
Since there are no new line characters, every 256 bytes represent a layer, every 128 layers represents a chunk.