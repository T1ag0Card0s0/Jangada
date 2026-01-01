# Quick Reference: Code Quality Commands

## During Normal Development

Simply compile as usual - formatting and linting happen automatically:

```bash
make              # Automatically formats and analyzes all C files, then compiles
make clean        # Clean build artifacts
```

## Standalone Code Quality Tasks

```bash
# Format all source code to project style
make format

# Run static analysis with automatic fixes
make tidy

# Run static analysis in read-only mode (see issues without fixing)
make tidy-check
```

## What Happens When You Compile

Each C/C++ file automatically goes through:

1. **FORMAT**: clang-format beautifies your code
2. **TIDY**: clang-tidy analyzes and auto-fixes issues
3. **COMPILE**: gcc compiles the cleaned-up code

Example output:
```
FORMAT kernel/task.c        <- Code is formatted
TIDY kernel/task.c          <- Code is analyzed & fixed
CC -c kernel/task.c         <- Code is compiled
```

## Configuration

- **Style Settings**: `.clang-format`
- **Analysis Rules**: `.clang-tidy`
- **Build Integration**: `config/make.global`

## Useful Workflows

### Review changes before committing

```bash
make format           # Format all code
git diff              # See what changed
git add <files>
git commit
```

### Check for code issues

```bash
make tidy-check       # Run analysis (read-only)
# Review warnings shown in output
```

### Auto-fix common issues

```bash
make tidy             # Auto-apply fixes
git diff              # Review changes
git add <files>
git commit
```

## Tips

- Build failures show both formatting and analysis messages
- Warnings don't block compilation - code still compiles
- clang-tidy auto-fixes are safe for most common issues
- Review major changes suggested by tidy before accepting

## Learn More

See `docs/CODE_QUALITY.md` for detailed documentation.
