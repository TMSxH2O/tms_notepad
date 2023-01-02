> https://developer.nvidia.com/blog/thinking-parallel-part-iii-tree-construction-gpu/

```c++
// 10-bit integer into 30 bits
uint32_t expandBits(uint32_t v)
{
    v = (v * 0x00010001u) & 0xFF0000FFu;
    v = (v * 0x00000101u) & 0x0F00F00Fu;
    v = (v * 0x00000011u) & 0xC30C30C3u;
    v = (v * 0x00000005u) & 0x49249249u;
    return v;
}
// 例如 3FF(0011 1111 1111) 最后会被调整为 0000 1001 0010 0100 1001 0010 0100 1001

// 16-bit integer into 32 bits
// 这个没找到直接用几个位运算直接得到的结果的方法，所以写了循环，感觉不如直接写成两个16bit合并成32bit Morton code 的方法
uint32_t expandBits(uint32_t v)
{
    uint32_t result = 0;
    for (uint32_t i = 0, j = 15; i < 32, j >= 0; ++i) {
        result <<= 1;
        if (i & 1) {
            result |= (v >> j--) & 1;
        }
    }
    return result;
}
```


