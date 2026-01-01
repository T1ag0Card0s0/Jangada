# Code Formatting & Linting Setup

Your Jangada RTOS project is now configured with automatic code formatting and static analysis. This document explains what was set up and how to use it.

## What's Installed

### 1. **clang-format** - Code Formatter
- Automatically formats C/C++ code to match project style
- Configuration: `.clang-format` (YAML format)
- Runs automatically during compilation

### 2. **clang-tidy** - Static Code Analyzer  
- Analyzes code for bugs, performance issues, and style violations
- Configuration: `.clang-tidy` (YAML format)
- Runs automatically during compilation with fixes applied
- Can also run standalone for detailed analysis

## Configuration Files Created

### `.clang-format`
Defines code style:
- **Style**: LLVM-based with 4-space indentation
- **Line Length**: 80 characters
- **Braces**: Allman style (braces on new line)
- **Control Statements**: Always explicit braces, no single-line statements
- **Includes**: Automatically sorted

### `.clang-tidy`
Defines static analysis rules:
- **Checks**: bugprone, readability, misc, clang-analyzer, performance, portability
- **Type Naming**: `_t` suffix (e.g., `my_type_t`)
- **Function Naming**: snake_case
- **Macro Naming**: UPPER_CASE
- **Warnings**: Displayed but don't block compilation

## Build Targets

### Automatic During Compilation

When you run `make`, each C/C++ file is:
1. **Formatted** with clang-format (in-place)
2. **Analyzed** with clang-tidy (with auto-fixes applied)
3. **Compiled** with the cross-compiler

Example:
```bash
make
```

Output includes:
```
FORMAT kernel/task.c
TIDY kernel/task.c
CC -c kernel/task.c -o kernel/task.c.o
```

### Standalone Commands

#### Format All Code
```bash
make format
```

Formats all C/C++ source files (`.c`, `.cpp`, `.h`, `.hpp`) to match the project style. Files are modified in-place.

#### Run Analysis with Fixes
```bash
make tidy
```

Runs clang-tidy on all source files and automatically applies suggested fixes. Warnings are displayed but don't block compilation.

#### Run Analysis (Read-Only)
```bash
make tidy-check
```

Runs clang-tidy for detailed analysis without modifying files. Useful for reviewing potential issues before accepting auto-fixes.

## How It Works

### During Build

Each C source file goes through this pipeline:

```
Source File (.c)
    ↓
[FORMAT] clang-format -i <file>
    ↓
[TIDY] clang-tidy --fix --fix-errors <file>
    ↓
[COMPILE] gcc -c <file> -o <object>
```

Both formatting and analysis steps can modify the source file. The modified version is then compiled.

### Make Integration

The build system rules in `config/make.global` automatically trigger:

```makefile
$(BUILD_DIR)/%.c.o: $(PROJECT_PATH)/%.c
    # Step 1: Format the file
    @clang-format -i $<
    # Step 2: Analyze and fix
    @clang-tidy --fix --fix-errors $< -- $(INCLUDES) $(CFLAGS)
    # Step 3: Compile
    @$(CC) $(OBJCFLAGS) $< -o $@
```

## Configuration Details

### clang-format Style Rules

```yaml
BasedOnStyle: LLVM           # Base on LLVM style
IndentWidth: 4               # 4 spaces per indent level
ColumnLimit: 80              # Line length limit
BreakBeforeBraces: Allman    # Opening brace on new line
AllowShortFunctionsOnASingleLine: None  # Functions on multiple lines
IndentCaseLabels: true       # Indent case labels
SortIncludes: true           # Sort #include statements
```

### clang-tidy Check Categories

| Category | Purpose |
|----------|---------|
| `bugprone-*` | Detect bug-prone patterns |
| `readability-*` | Code readability issues |
| `misc-*` | Miscellaneous checks |
| `clang-analyzer-*` | Static analysis |
| `performance-*` | Performance suggestions |
| `portability-*` | Cross-platform compatibility |

### Example Checks

- **bugprone-easily-swappable-parameters**: Warns when parameters can be accidentally swapped
- **bugprone-macro-parentheses**: Requires parentheses around macro replacement lists
- **readability-identifier-naming**: Enforces naming conventions (snake_case functions)
- **performance-unnecessary-copy**: Detects inefficient copying operations

## Example Workflow

### 1. Make a Change

Edit a source file:
```c
// Before
void   badly_formatted_function  (   int x,   int y   )   {
}

#define MY_CONSTANT -1
```

### 2. Build the Project

```bash
make clean
make
```

### 3. File is Automatically Tidied

The file is modified to:
```c
// After
void badly_formatted_function(int x, int y)
{
}

#define MY_CONSTANT (-1)
```

### 4. Warnings Shown

During build, analysis warnings appear:
```
TIDY kernel/task.c
kernel/task.c:10:20: warning: easily-swappable parameters...
```

### 5. Code Compiles Successfully

Even with warnings, compilation continues and succeeds.

## Customization

### Modify Formatting Style

Edit `.clang-format`:

```yaml
# Change indentation to 2 spaces
IndentWidth: 2

# Change line length to 100
ColumnLimit: 100

# Use tabs instead of spaces
UseTab: ForIndentation
```

Then reformat:
```bash
make format
```

### Modify Static Analysis Rules

Edit `.clang-tidy`:

```yaml
Checks: >
  -*,
  bugprone-*,
  readability-*,
  # Add more checks here
  cppcoreguidelines-*
```

### Disable Specific Checks

In `.clang-tidy`, exclude checks:

```yaml
Checks: >
  -*,
  bugprone-*,
  -bugprone-easily-swappable-parameters,  # Disable this one
  readability-*
```

## Troubleshooting

### clang-format not found

Install clang-format:
```bash
# Ubuntu/Debian
sudo apt-get install clang-format

# macOS
brew install clang-format
```

### clang-tidy not found

Install clang-tools:
```bash
# Ubuntu/Debian
sudo apt-get install clang-tools

# macOS
brew install clang-tools
```

### Tools not in PATH

Specify full paths in Makefile:
```bash
make CLANG_FORMAT=/usr/bin/clang-format CLANG_TIDY=/usr/bin/clang-tidy
```

### Compilation failing due to analysis errors

The current setup shows warnings but doesn't block compilation. If you want to treat warnings as errors:

Edit `.clang-tidy`:
```yaml
WarningsAsErrors: '*'  # Convert all warnings to errors
```

Then clang-tidy will block compilation on warnings.

## Performance Notes

- **Format + Tidy**: Adds ~1-2 seconds per source file
- **For large projects**: Consider using parallel builds (`make -j4`)
- **Skip formatting**: Remove clang-format step from `config/make.global` if speed is critical

## Best Practices

1. **Review auto-fixes**: Check what clang-tidy changes before committing
   ```bash
   make tidy
   git diff  # Review changes
   ```

2. **Fix warnings**: Address legitimate warnings shown by clang-tidy
   ```bash
   make tidy-check  # See all issues
   ```

3. **Commit formatted code**: Format before committing
   ```bash
   make format
   git add <files>
   git commit
   ```

4. **CI/CD integration**: Run `make tidy-check` in CI to catch issues
   ```bash
   # In CI pipeline
   make tidy-check || exit 1
   ```

## See Also

- [clang-format Documentation](https://clang.llvm.org/docs/ClangFormat/)
- [clang-tidy Documentation](https://clang.llvm.org/extra/clang-tidy/)
- [Project README](../README.md)
- [Build System Guide](../docs/BUILD_SYSTEM.md)
