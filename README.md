# Enigma

## 背景介绍

恩尼格玛是对二战时期纳粹德国使用的一系列相似的转子机械加解密机器的统称，它包括了许多不同的型号，为密码学对称加密算法的流加密。

## 实验原理

Enigma机有一个键盘和一系列的转轮, 键盘用来输入明文字符串, 每个转轮是字母的任意组合, 有26个接线端, 用来完成一种简单的替换, 同时每个转轮的输出端连接到其下一个相邻转轮的输入端. 当输入一个明文字符时, 相应的信号从第一个轮转的输入端进入, 依次经过各个相邻的转轮,当每经过一个转轮的时候, 该转轮会转动一个位置, 当该转轮装动了一个周期之后, 其下一个转轮会转动一个位置, 这样一直到达最后一个转轮, 然后反射回来, 即可得到密文

![img](https://wuhlan3-1307602190.cos.ap-guangzhou.myqcloud.com/img/6.jpg)

Enigma机有什么特点呢？

- 使用了三个转子。每个转子有`26`个数字，所以最终的加密周期为`26*26*26`
- 转动具有传递机制。即当一个转子转动一个周期之后，才会带动第二个转子转动，以此类推。所以属于多表代替密码
- 转子可以拆卸。这就导致了更多可能的出现。最终秘钥的决定因素有：3个转子的起始位置, 3个转子的布线种类, 连接板的布线类型.
- 反射板。反射板是两个字母的一一映射。这使得每一个字母加密之后都不会是它本身。

## 实验过程

### 1.转子的实现：

涉及的参数与函数如下：

- `order`：记录了转子中的偏移量，大小为26。
- `orderOrig`：记录了shuffle过后转子中的偏移量，大小为26。用以重置
- `shffle()`：使用了Fisher-Yates洗牌算法，对转子的偏移量顺序进行打乱。
- `reset()`：根据orderOrig来对order进行重置。
- `find()`：用以逆向编码

```c++
class Roter
{
public:
    vector<int> order;  
    vector<int> orderOrig;
    Roter()
    {
        order.resize(26);
        orderOrig.resize(26);
        for (int i = 0; i < order.size(); i++)
        {
            order[i] = i;
        }
    }
    //打乱order，
    void shuffle()
    {
        for (int i = 0; i < order.size(); i++)
        {
            int j = i + rand() % (order.size() - i);
            swap(order[i], order[j]);
        }
        for (int i = 0; i < order.size(); i++)
        {
            orderOrig[i] = order[i];
        }
    }
    void reset()
    {
        for (int i = 0; i < order.size(); i++)
        {
            order[i] = orderOrig[i];
        }
    }
    int find(int index){
        for(int i = 0; i < order.size(); i++){
            if(order[i] == index)return i;
        }
        return -1;
    }
};
```

### 2.转子组的实现：

这一个类实现起来比较简单。仅仅是把多个转子类封装起来。

提供了如下函数：

- `append()`添加一个转子

- `print()`显示所有转子的信息
- `reset()`对所有转子进行重置

```c++
class RoterList
{
public:
    vector<Roter> roterarr;

    RoterList()
    {
    }
    void append(Roter r)
    {
        roterarr.push_back(r);
    }
    void print()
    {
        cout << "Roter list:" << endl;
        for (int i = 0; i < roterarr.size(); i++)
        {
            cout << "[ ";
            for (int j = 0; j < roterarr[i].orderOrig.size(); j++)
            {
                cout << roterarr[i].orderOrig[j] << " ";
            }
            cout << " ]" << endl;
        }
    }
    void reset()
    {
        for (int i = 0; i < roterarr.size(); i++)
        {
            roterarr[i].reset();
        }
    }
};
```

### 3.反射器的实现

虽然是Enigma类里的一个一个方法。不过也算是机器的一个单独零件，所以拎出来讲讲。其特点就是两个字母互相映射。这样当每一个信号到达反射板时, 信号会沿路回去的，任意一个字母加密后都不会是它本身。

最终的效果如下：

```
A: H B: P C: W D: G E: I F: R G: D H: A I: E J: L K: O L: J M: Y 
N: Q O: K P: B Q: N R: F S: U T: X U: S V: Z W: C X: T Y: M Z: V
```

```c++
void Enigma::setReflector(string path)
    {
        vector<char> arr(26);
        for (int i = 0; i < arr.size(); i++)
        {
            arr[i] = 'A' + i;
        }
        for (int i = 0; i < arr.size(); i++)
        {
            int j = i + rand() % (arr.size() - i);
            swap(arr[i], arr[j]);
        }
        for (int i = 0; i < 13; i++)
        {
            reflector[arr[i]] = arr[i + 13];
            reflector[arr[i + 13]] = arr[i];
        }
        ofstream fout;
        fout.open(path, ios::out);
        for (int i = 0; i < 26; i++)
        {
            fout << char(i + 'A') << ": " << reflector[i + 'A'] << endl;
        }
        fout.close();
    }
```

### 4.接线板的实现

与反射器的实现类似，就不再赘述。相当于进行了一次单表代替加密与解密。

### 5.Enigma机的实现

<img src="https://wuhlan3-1307602190.cos.ap-guangzhou.myqcloud.com/img/image-20211208143935774.png" alt="image-20211208143935774" style="zoom:50%;" />

涉及的函数与参数如下：

```c++
class Enigma
{
    RoterList rlist;	//转子组
    string inputtext;	//输入的文本
    string outputtext;	//输出的文本
    unordered_map<char, char> plugboard;  //反射器

public:
    Enigma(RoterList &rl); 			//需要准备转子组来构造函数
    void input(string &str); 		//输入文本
    void setPlugboard(string path); //设置接线板
   	void setReflector(string path);	//设置反射器
    void step(int sign, char &c);	//解密的分步
    void encipherAndDecipher();		//加密或解密
    void print_inputtext();			
    void print_outputtext();
    void reset();					//重置密码机，为解密做准备
    void move(Roter &r);			//转动转子
    void linkAndMoveRotors(int counter); 	//转动转子，并且连带其他转子转动
    string get_outputtext();		//输出文本
};
```

选取比较重要的几个函数来说说：

#### (1)step-经过三个转子

经过三个转子的过程。根据上面的原理图可以看到，依次经过每一个转子，根据字符来获取转子中映射的下一个字符。由于是有来回两次的编码过程，所以使用`sign == 1`来表示`来`的过程，使用`sign==-1`表示`回`的过程。

```c++
void step(int sign, char &c)
{
    if(sign == 1){
        int index = int(c - 'A');
        for (auto roter : rlist.roterarr)
        {
            index = roter.order[index];
        }
        c = char(index + 'A');
    }else{
        int index = int(c - 'A');
        for (int i = rlist.roterarr.size()-1; i >= 0 ; i--)
        {
            index = rlist.roterarr[i].find(index);
        }
        c = char(index + 'A');
    }

}
```

#### (2)转动转子

对一个转子进行转动，如果该转子的顺序与最初转子`orderOrig`的顺序一样的话，就对下一个转子进行转动（使用递归的方式）。

```c++
void linkAndMoveRotors(int counter)
    {
        if (counter >= 3)
            return;
        move(rlist.roterarr[counter]);
        if (rlist.roterarr[counter].order[0] == rlist.roterarr[counter].orderOrig[0])
        {
            linkAndMoveRotors(counter + 1);
        }
    }
```

#### (3)解密/加密

如下面的代码所示：

```c++
void encipherAndDecipher()
{
    for (auto c : inputtext)
    {
        c = plugboard[c];	//经过插线板
        step(1, c);			//顺次通过三个转子
        c = reflector[c];	//经过反射器
        step(-1, c);		//反向通过三个转子
        c = plugboard[c];	//经过插线板
        outputtext.push_back(c);  //将加密/解密后的字符，加入ouput字符串钟
        linkAndMoveRotors(0);	//旋转转子
    }
}
```

## 实验结果

实验结果正确。可以看到可以正常加密并正确解密。且具有enigma机的特点，即`HELLO`中的两个`L`加密成了不同的字母。

![image-20211208114535316](https://wuhlan3-1307602190.cos.ap-guangzhou.myqcloud.com/img/image-20211208114535316.png)

此外，由于反射器的作用，任意一个字母都不会被加密成它本身。

![image-20211208142837243](https://wuhlan3-1307602190.cos.ap-guangzhou.myqcloud.com/img/image-20211208142837243.png)

