# maze routing project

### 简介

这是一个用于解决特定输入下的maze routing problem的项目。

输入数据是10000$\times$10000的数字矩阵，数字权值范围是{-1，1，2，3，4，5}。其中-1表示无穷大。

**历史上**，关于这类问题有**搜索和DP**两大类解法，搜索包括DFS、BFS、双端BFS、A*等算法，DP则包括各种**最短路算法**的变体，类似地可以分为记忆化搜索、类BFS最短路(实质是模拟Dijkstra)。

由于本项目针对的数据较大，因此我认为基于搜索的算法不太现实。在本项目中，我使用了一种**基于双端BFS模拟Dijkstra的算法**，并且由于地图权值较小，我使用了**流水线**的思想进行模拟，最后在每级流水线中使用**数据并行**进行了多线程加D速。

使用的并行库是OpenMP，选择它的理由有两点。首先，我认为类最短路问题不太适合使D用GPU求解，因此没有多大必要使用OpenCL和CUDA。其次，我的算法每级流水中的任务数是O(n)级别的（这里n=10000），划分给每个CPU的数据量并不多，如果使用进程间通讯，通讯代价过高。因此，最终我选择了OpenMP做并行加速。

### 文件目录结构

代码位于./src下，其中./src/lib/中为一些库函数，./src/target/中为实际的算法文件，其下共有三个算法文件，分别为maze_routing_violence.cpp、maze_routing_st.cpp、maze_routing_mt.cpp，分别对应了三个可执行文件，分别代表了**单端算法**、**双端算法**、以及**并行化的双端算法**。

其中**双端算法（maze_routing_st）**是下面将会提到的**线性算法**，而**单端算法（maze_routing_violence）**仅仅作为标准答案的提供者（用来对拍），**并行化的双端算法（maze_routing_mt）**是最终的**并行算法**。

目标文件位于./build中，测试文件位于./benchmark中，预先测试汇总的数据位于./statistic中。

### 运行方式

在目录下使用`make`会构建所有程序。

你可能需要先准备`./benchmark`目录，以及其下的测试文件。

运行`make run`会使用`./benchmark`目录下的所有测试文件测试最终的并行算法，计算结果保存在`./result.txt`，运行时间等信息会输出到屏幕。

运行`make run_all`会依次测试以下提到的三个算法，但计算结果不会保存到`./result.txt`，而是会保存到`./result_XXX.txt`，`XXX`对应了算法名称。

### 线性算法

观察到数据有两个显著的**特点**：

​	1.这是**点权图**，意味着在通用的Dijkstra中，一个点一旦被更新，就再也不需要更新，因为所有邻居节点更新当前节点的额外代价是相同的。（换句话说，一旦入队，就再也不需要入队）

​	2.**点权最大为5**，如此小的点权范围，这意味着我们可以根据点权进行迭代，这是常规最短路问题做不到的。

正是因为上面的**第2点**，我们可以使用双向Dijkstra。具体来讲，相当于Dijkstra算法开始时，同时在优先队列中（我的算法实际没有使用优先队列）放入起点S和终点D，并将其距离均设为0。这样，出队的点会形成两个联通集，一个包含S，一个包含D。两个集合不断扩张，当二者**“接触”**时，由于上面的**第2点**，我们可以断定，两边最多再扩张**4（最大点权-1）**的距离后，**一定可以从两集合外沿所有接触的点对中找出位于最短路上的两个点**。注意两集合是不交的，禁止一方拓展另一方已有的节点，因此**接触点对**的数量是O(n)的(n=10000)。

上面我们提到了，算法实际没有使用优先队列，这是上面的**第1点**和**第2点**的共同结果。**第1点**使得每个点只需更新一次。**第2点**使我们可以维护**距离桶**，每个桶对应一个唯一的距离，所有距离相同的点位于同一个桶。这样，我们只需要从小到大遍历桶即可，而无需使用优先队列，这样遍历的行为类似流水线的处理。

事实上，我们只需要**MAX_VAL+1**个桶（MAX_VAL表示最大权值），构成一个**环**。处理每个桶时，首先需要从桶中取出节点，更新邻居，然后再将已更新的邻居节点放置到后面的**MAX_VAL**个桶的某个中。

### 暴力对拍算法

这个算法仅仅用于提供标准答案，只需要将**线性算法**中的双端改成单端即可。

### 并行化算法

基于**线性算法**，将上面的**距离桶**（或者流水线）中待处理的节点进行数据并行即可。需要注意的是同步问题，同时还需兼顾效率。使用pragma omp for会自动将数据遍历进行拆分，我为每个线程分配了**MAX_VAL+1**个私有桶，这样减少了公共内存的写操作，从而减少了缓存的miss。计算结束后，使用prgama omp critical和pragma omp single将私有桶合并到公共桶。

### 实验结果

在我的个人PC上运行，使用intel core i7-9750h处理器，6核心12线程。

加载地图的时间恒定为每个测试点0.8s左右，因为地图大小是恒定的。由于地图较大，我使用了fread+手写getchar+手写整数解析的方法，0.8s对于读取并分析约200mB的数据而言还是很快的。

以下表格列出了算法运行时间：
（已排除了加载地图的时间）

||单端线性算法|双端线性算法|双端并行化算法|
|---|---|---|---|
|总时间（s）|135.37|122.07|27.03|
|case的平均时间（s）|2.71|2.44|0.54|

并行算法的平均加速比：
（平均加速比：每个case加速比的平均值，**并非使用平均时间计算**）

|相对于|单端线性算法|双端线性算法|
|---|---|---|
||5.51|4.44|

我的线性算法复杂度为O($n^2$)，特别的，在权值相同的图上，为O($d^2$)，d为S和D的距离。对于并行算法，把最大权值看成常数，那么一个桶的处理时间O(n/p)，合并的时间为O(p+n/p)，桶的个数是O(n)，故最终复杂度为O($n^2/p+np$)。

从结果来看，6核CPU能得到4.44的加速比，我认为整体是不错的。