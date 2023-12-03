# Computer Graphic Rookie project
This is the complete project (i.e means you can get a perfect score) of XJTU COMP551805(Computer Graphic), which includes rendering, geometry, and physical simulations. 

<img src="./实验依赖关系图.png" width="400" />

# 这边我做了下面四个实验
### 2.1 欧拉角到四元数

### 2.2 软渲染器：局部光照
### 2.7 基于半边的局部操作
### 2.12 前向欧拉法模拟运动
### 2.13 其他方法解刚体运动

<br>
具体的实验内容均可以在实验文档中找到<br><br>
实验环境: dandelion （已fork） <br>

下面简要说明一下dandelion的操作过程


```bash
$ mkdir build # 创建一个名为build的目录
$ cd build    # 切换到刚创建的build目录
$ cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug 
$ cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release
# -S选项后面跟的是源目录的路径。在这里，..表示父目录，
#-B选项指定了构建目录，即CMake将在哪里生成构建文件（如Makefile）。这里的.表示当前目录，意味着构建文件将在当前目录中生成。
$ cmake --build . --parallel 8               # 并行构建项目，使用8个线程
```

`cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug`:  
这个命令配置了一个用于调试的构建。在调试构建中，编译器不会进行太多优化，以便更容易地进行调试（例如，可以更容易地进行步进调试，检查变量值等）。调试构建通常包含额外的调试信息，使得在出错时可以提供更详细的错误报告和调试支持。这种构建类型更适合开发和测试过程中，因为它可以帮助开发者发现和修复bug。

`cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release`:  
这个命令配置了一个用于发布的构建。在发布构建中，编译器会进行更多的优化，以提高程序的运行速度和效率。这些优化可能包括减少程序大小、提高执行速度等。由于优化的原因，调试信息可能会被去除或减少。发布构建适合最终用户使用，因为它提供了更好的性能。

总之，区别在于编译器优化和调试信息的水平。调试构建适合开发和调试阶段，而发布构建适合最终交付给用户的产品。

