#### Application（env）:

​	       Visual studio2019  \  C++  \  MFC  \  socket

#### The Main Func

This is an application to chat. Visual programming realizes the chat program based on C/S mode. The program uses DH key agreement protocol to negotiate the key, which encrypts the message as a symmetric key. After receiving the message, the receiver can calculate whether the message has been tampered according to the hash.

- Using **Socket** to create a connection.
- Using **Diffie-Hellman** to exchange the des_key.
- Using **DES** to keep a secret.
- Using **Hash** to identify the message which is credible or not.

#### How To Use:

​        After U download the zip file ,open the `.sln` file  with  VS.

​       The main function was written in `Dlg.cpp`.

#### Warnings:

​       The  task was my homework in the university when I am junior .Please delete it within the 24 hours.

------

#### 应用程序（环境）:

Visual studio2019  \  C++  \  MFC  \  socket

#### 主要功能：

可视化编程实现基于CS模式的聊天程序，程序采用DH密钥协商协议来商定密钥，该密钥作为对称密钥对消息实现加密，接收方收到消息后，可以根据hash推算出，消息是否被篡改。

- 使用socket建立连接，保持通信
- 使用DH密钥协商协议商定密钥
- 使用DES加密算法保证消息机密
- 使用Hash杂凑函数验证可信性

#### 如何使用

下载我的代码，文件夹中有`.sln` 的文件，双击使用VS打开。

主要功能实现在`Dlg.cpp`中

#### 警告：

这是本人学校时的作业，仅作学习借鉴，请下载24小时内删除。