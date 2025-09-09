package main

import (
	"flag"
	"fmt"
	"os"

	"chtl/CHTL/context"
	"chtl/CompilerDispatcher"
)

func main() {
	var (
		inputFile  = flag.String("i", "", "输入文件路径")
		outputFile = flag.String("o", "", "输出文件路径")
		watch      = flag.Bool("w", false, "监听模式")
		debug      = flag.Bool("debug", false, "调试模式")
		help       = flag.Bool("h", false, "显示帮助信息")
	)
	flag.Parse()

	if *help {
		showHelp()
		return
	}

	if *inputFile == "" {
		fmt.Println("错误: 请指定输入文件")
		showHelp()
		os.Exit(1)
	}

	// 创建编译上下文
	ctx := context.NewContext()
	ctx.SetDebugMode(*debug)

	// 创建编译器调度器
	dispatcher := CompilerDispatcher.NewDispatcher(ctx)

	// 执行编译
	if *watch {
		err := dispatcher.WatchAndCompile(*inputFile, *outputFile)
		if err != nil {
			fmt.Printf("编译错误: %v\n", err)
			os.Exit(1)
		}
	} else {
		err := dispatcher.Compile(*inputFile, *outputFile)
		if err != nil {
			fmt.Printf("编译错误: %v\n", err)
			os.Exit(1)
		}
		fmt.Println("编译完成!")
	}
}

func showHelp() {
	fmt.Println("CHTL 编译器")
	fmt.Println("用法: chtl [选项]")
	fmt.Println("")
	fmt.Println("选项:")
	fmt.Println("  -i string    输入文件路径")
	fmt.Println("  -o string    输出文件路径")
	fmt.Println("  -w           监听模式")
	fmt.Println("  -debug       调试模式")
	fmt.Println("  -h           显示帮助信息")
	fmt.Println("")
	fmt.Println("示例:")
	fmt.Println("  chtl -i example.chtl -o output.html")
	fmt.Println("  chtl -i example.chtl -w")
}