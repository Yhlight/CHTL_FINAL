import subprocess
import sys

tests = [
    "FullPipelineTest",
    "LocalStyleTest",
    "AttributeArithmeticTest",
    "ElementTemplateTest",
    "VarTemplateTest"
]

all_passed = True

for test in tests:
    print(f"\n--- Running test: {test} ---\n")

    # Build the test
    build_process = subprocess.run(["python3", "build.py", test], capture_output=True, text=True)
    if build_process.returncode != 0:
        print(f"FAILED to build {test}")
        print(build_process.stderr)
        all_passed = False
        continue

    print(f"Build successful for {test}.")

    # Run the test
    run_process = subprocess.run([f"./{test}_executable"], capture_output=True, text=True)
    if run_process.returncode != 0:
        print(f"FAILED to run {test}")
        print(run_process.stderr)
        all_passed = False
    else:
        print(f"Run successful for {test}.")
        print("Output:")
        print(run_process.stdout)

print("\n--- Test Summary ---")
if all_passed:
    print("All tests passed successfully!")
else:
    print("Some tests failed.")
    sys.exit(1)
