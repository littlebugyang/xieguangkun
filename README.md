# xieguangkun
2019年华为云鲲鹏开发者大赛128强-谢广鲲

### 0. 致谢

- 感谢提供C++ Demo的[作者](https://github.com/lx852357/hw_cloud_client)
- 感谢我的另外两个队友, 一个是我们队伍的[核心算法提供者](https://github.com/201630666165), 另一个应该是[论坛上首先完成了10篇帖子的大佬](https://github.com/Guest666666667)
- 感谢华为云提供的比赛机会和所有幕后的工作人员



### 1. 程序思路

谢广鲲的程序实现完全是按照 **G67** 大佬的10篇帖子来写的, 进攻、防守、巡游, 这些都已经在程序中实现. 



### 2. 最后一战

由于我们全队不怎么了解C++, 而我们三个唯一的共同语言(编程语言)就是仅学习了一个学期的C++. 于是我们就愉快地开始了XJB`new`但是不带`delete`的编程~~

我们在Debug的时候就眼睁睁地看着内存值莫名其妙地变成这个那个... 到复赛第一轮正式比赛的当天, 我们都还没有意识到内存泄漏的问题, 所以, 谢广鲲可能是所有队伍中唯一一个在那天还提交弟弟版本的队伍....



### 3. 预览

可以用两种方式生成可执行程序.

#### (1) Visual Studio IDE 生成

- 将一级目录下的`run_local.bat`放到与`server`, `ui`等文件放到同一级目录下. 就像这样: 

  ```
  +-- _server
  |   +-- replay.txt
  |   +-- ...
  +-- _ui
  |   +-- nw.bat
  |   +-- ...
  +-- run_local.bat
  ```

- 运行`run_local.bat`

- 打开项目目录下的`xieguangkun.sln`, 点击`本地Windows调试器`进入Debug模式(谁知道还会不会内存泄漏呢~
- 等待黑框框不再输出"msg..."之后, 就可以打开`./ui/nw.bat`查看对战结果

#### (2) CMake生成

- 按照C++ Demo的[作者](https://github.com/lx852357/hw_cloud_client)仓库中的ReadMe相应的步骤即可

  ```shell
  $ sh ./compile.sh
  ```

- 生成的可执行文件在`./build`下



### 4. 其他

项目是新建的GitHub仓库, 所以没有之前代码迭代的辣眼睛commit message. 内存泄漏的问题我花了一个下午排查一下, 如果还出现奇奇怪怪的鲲鲲主动求死等情形... 那也正常, 毕竟我们太菜了. 

除此之外, 由于还有很多想法因为时间和实力问题没有实现, 所以经常会看到有我方鲲鲲一直追着人家跑的情景, 对, 就是这么菜.

对代码有疑惑, 或者各位想要吐槽代码风格和程序设计的话, 欢迎提issue.

