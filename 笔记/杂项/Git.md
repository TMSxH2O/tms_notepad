# 常用命令

```bash
# 查看当前库的远程链接
git remote -v

# 当前库的子库，其中比较常用的是 
#     git submodule update --init
# 效果等价于 git clone --recursive xxx/xxx
git submodule -h
```

## 更新submodule的路径

```shell
# 修改子模块的链接
git submodule set-url <SubmoduleName> <Url>

# 修改后，需要先同步路径
git submodule sync

# 最后重新更新
# 通常会加上 -f 来强制更新
git submodule update
```