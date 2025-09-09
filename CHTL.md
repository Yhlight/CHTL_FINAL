# 关于CHTL
CHTL是基于C++语言实现的超文本语言，其本质是为了提供一种更符合开发者编写HTML代码的方式，使用MIT开源协议  

## 注释
在CHTL中，使用//表示注释  
使用/**/代表多行注释  
使用--代表会被生成器识别的注释  
//和/**/注释不会被生成器所识别，生成的HTML不会带有这些注释  
而--注释则会根据上下文生成不同编程语言类型的注释  

## 文本节点
在CHTL中，使用text { }表示一段文本

```chtl
text
{
    "这是一段文本"
}
```

除此之外，你也可以使用text属性来表示一段文本  
text属性只能在元素内部使用，而text { }可以在元素外使用  

```chtl
div
{
    text: "这是一段文本";
}
```

## 字面量
CHTL支持text与属性值使用无修饰字面量(没有引号的字符串)，这允许你像CSS一样无需书写引号  
除此之外，CHTL支持双引号字符串("")，单引号字符串('')  

```chtl
text
{
    这是一段文本
}

style
{
    color: red;
}
```

## CE对等式
CE对等式即Colon = Equal  
即在CHTL之中，':'与'='完全等价  
你可以在一些推荐的情景下使用'='  

## 元素节点
CHTL支持HTML所有的元素，无论是单标签还是双标签，还是块级，行内，行内块元素  

```chtl
html
{
    head
    {

    }

    body
    {
        div
        {
            text
            {

            }
        }

        span
        {

        }
    }
}
```

## 属性
在CHTL中，使用`属性名 : "属性值";`表示属性  

```chtl
div
{
    id: box;
    class: welcome;

    text
    {
        HelloWorld
    }
}
```

## 局部样式块
CHTL对<style></style>进行了改进，允许开发者在元素的内部嵌套style {}，以此进行一些css操作  
包括但不限于内联样式，类选择器，id选择器，伪类选择器，伪元素选择器  
类选择器，id选择器，伪类选择器，伪元素选择器相关的代码将会被自动添加至全局样式块之中  
注意！CHTL并没有对全局样式块进行增强，请不要尝试在全局样式块使用局部样式块的功能  

### 内联样式
你可以在style{}内部直接添加属性，这些属性会成为元素的内联样式  

```chtl
body
{
    div
    {
        // 内联样式
        style
        {
            width: 100px;
            height: 200px;
        }
    }
}
```

### 自动化类名 / id
你可以直接在局部样式块中使用类 / id选择器  
无需手动编写class / id  
CHTL会自动为元素添加类名 / id  

```chtl
div
{
    style
    {
        .box  // 自动添加类名box给元素，且整部分代码将自动添加至全局样式块
        {
            width: 300px;
        }
    }
}
```

### 上下文推导
你可以在局部样式块之中使用&表示类名 / id(优先类名)  
&将会根据上下文来推导成类名 / id，例如检查元素的class / id  

这里有两种使用方式，一种是上文使用class / id  
下文使用&，&:hover，&::before代替类 / id选择器  
CHTL并不建议使用这一种方式  

而是建议下述这一种使用方式  
不写元素的class / id，直接使用.box / #box，让CHTL自动添加类名 / id  
将&用于简化伪类选择器(&hover)与伪元素选择器(&::before)的使用  

```chtl
div
{
    style
    {
        .box  // 让CHTL自动添加类名
        {

        }

        &:hover  // 使用&简化伪类的使用，之后&会被解析成类名，然后整体被添加至全局样式块
        {

        }
    }
}
```

### 属性条件表达式  
元素自身的属性的表达式 ? 选项 : 选项  

div
{    
    style
    {
        width: 100px;
        height: 100px;
        background-color: width > 50px ? "red" : "blue";
    }
}

#### 链式调用  
div
{   
    style
    {
        width: 100px;
        height: 100px;
        background-color: 
                width > 50px ? "red" : "blue",
                width > 100px ? "green" : "yellow", 
                height < 100px ? "purple" : "pink";
    }
}

#### 可选选项  
div
{    
    style
    {
        width: 100px;
        height: 100px;
        background-color: 
                width > 50px ? "red",
                width > 100px ? "green", 
                height < 100px ? "purple" : "pink";
    }
}

#### 逻辑运算符
逻辑与(&&)与逻辑或(||)
属性表达式允许使用逻辑与(&&)与逻辑或(||)  

```chtl
div
{   
    style
    {
        width: 100px;
        height: 100px;
        background-color: 
                width > 50px && width < 100px ? "red",
                width > 100px || height < 50px ? "green", 
                height < 100px ? "purple" : "pink";
    }
}
```

#### 指向属性条件表达式
属性条件表达式是支持参考不同元素的属性来进行条件表达的  
语法为CSS选择器.属性  
处于性能考虑，属性条件表达式只支持下述选择器  
box  自动推断，tag -> id -> class  
.box 类选择器  
#box id选择器  
button tag选择器  
.box button 后代选择器  
button[0] 精确访问  

```chtl
div
{
    class: box;
    style
    {
        width: 100px;
    }
}

div
{
    style
    {
        width: .box.width > 50px ? .box.width / 2 : .box.height / 2;
    }
}
```