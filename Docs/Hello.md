## opengl

> 是一种图像api

+ 有过两种模式： 

  + 立即模式（3.2之前）： 固定渲染管线
  + 核心模式（3.2开始）：完全抛弃移除旧特性

+ 基元类型： 基本类型前加了GL前缀

  + 比如：`GLfloat`

  + 使用opengl时使用其基元类型可保证**基元类型内存布局的平台无关性**

## pass

+ 大致流水线： 3D顶点输入 =》 顶点着色器 =》 进行图元装配 =》 几何着色器 =》 光栅化 =》 片段着色器进行着色 =》 测试和混合