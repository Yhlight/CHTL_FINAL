package main

import (
	"bufio"
	"chtl/pkg/generator"
	"chtl/pkg/lexer"
	"chtl/pkg/parser"
	"flag"
	"fmt"
	"io/ioutil"
	"os"
)

func main() {
	inputFile := flag.String("input", "", "The input .chtl file to compile.")
	outputFile := flag.String("output", "", "The output .html file. (default: stdout)")
	flag.Parse()

	if *inputFile == "" {
		fmt.Println("Error: Input file must be specified with -input flag.")
		flag.Usage()
		os.Exit(1)
	}

	content, err := ioutil.ReadFile(*inputFile)
	if err != nil {
		fmt.Printf("Error reading input file: %s\n", err)
		os.Exit(1)
	}

	// --- Compilation Pipeline ---
	l := lexer.New(string(content))
	p := parser.New(l)
	program := p.ParseProgram()

	if len(p.Errors()) != 0 {
		fmt.Println("Parser errors encountered:")
		for _, msg := range p.Errors() {
			fmt.Printf("\t- %s\n", msg)
		}
		os.Exit(1)
	}

	// Generate HTML
	gen := generator.New()
	html, err := gen.Generate(program)
	if err != nil {
		fmt.Printf("Error during HTML generation: %s\n", err)
		os.Exit(1)
	}

	// --- Output ---
	if *outputFile != "" {
		f, err := os.Create(*outputFile)
		if err != nil {
			fmt.Printf("Error creating output file: %s\n", err)
			os.Exit(1)
		}
		defer f.Close()

		writer := bufio.NewWriter(f)
		_, err = writer.WriteString(html)
		if err != nil {
			fmt.Printf("Error writing to output file: %s\n", err)
			os.Exit(1)
		}
		writer.Flush()
		fmt.Printf("Successfully compiled %s to %s\n", *inputFile, *outputFile)
	} else {
		fmt.Println(html)
	}
}
