# VBC - Very Basic Compiler

## Overview

VBC (Very Basic Compiler) is a mathematical expression parser and evaluator that implements a recursive descent parser to build an Abstract Syntax Tree (AST). The compiler handles basic arithmetic operations with proper operator precedence and parenthetical grouping.

### Supported Operations
- **Addition** (`+`)
- **Multiplication** (`*`)
- **Parenthetical grouping** (`()`)

### Input Constraints
- Operands: Single digits (0-9)
- Expression length: Variable
- Syntax: Standard infix mathematical notation

## Core Concepts

### Left-Associativity

Left-associativity defines the evaluation order for operators of equal precedence, ensuring mathematical correctness in expression parsing.

**Example**: `1 + 2 + 3` is parsed as `((1 + 2) + 3)`

This concept directly influences AST construction, where each new operator node adopts the previously constructed subtree as its left child.

### AST Construction Algorithm

The parser constructs the AST incrementally using a bottom-up approach:

#### Step 1: Initial Binary Operation
```
Expression: 1 + 2
AST:
      +
     / \
    1   2
```

#### Step 2: Chain Extension
```
Expression: 1 + 2 + 3
AST:
        +
       / \
      +   3
     / \
    1   2
```

The algorithm ensures that:
1. Each new operator node becomes the root
2. The previous AST becomes the left subtree
3. The new operand becomes the right child
4. Left-associativity is preserved

## Data Structures

### Node Structure

```c
typedef struct node {
    enum {
        ADD,     // Addition operator node
        MULTI,   // Multiplication operator node  
        VAL      // Value leaf node
    } type;
    int val;           // Node value (for VAL nodes)
    struct node *l;    // Left child pointer
    struct node *r;    // Right child pointer
} node;
```

**Field Descriptions:**
- `type`: Discriminator enum defining node operation or value type
- `val`: Integer value storage (used only for `VAL` type nodes)
- `l`: Pointer to left child node
- `r`: Pointer to right child node

### Memory Management

```c
void destroy_tree(node *n)
{
    if (!n)
        return;
    if (n->type != VAL) 
    {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}
```

**Algorithm:**
1. **Base Case**: Return if node is NULL
2. **Recursive Case**: For non-leaf nodes, recursively destroy children
3. **Cleanup**: Free current node memory

The function leverages the fact that `VAL` nodes are always leaves (no children to destroy).

## Parser Architecture

### Entry Point: `parse_addition(char *s)`

The primary parsing function that initiates the recursive parsing process and handles:
- Expression validation
- Operator precedence management
- Parenthetical grouping
- Error detection and reporting

### Parsing Strategy

The parser implements a **layered recursive descent** approach:

#### Layer 1: Primary Expression Validation
- **Numeric Literal**: Creates `VAL` node (leaf)
- **Parenthetical Expression**: Recursively calls `parse_addition()` for sub-expression

#### Layer 2: Multiplication Handling
- Continues processing within current precedence level
- Generates right operand node for multiplication operations

#### Layer 3: Addition Processing
- Returns to `parse_addition()` for lower precedence operations
- Creates addition node with previous AST as left child
- Validates subsequent tokens for continued parsing

## Error Handling

### Error Types and Messages

| Error Condition | Output Message | Exit Code |
|----------------|----------------|-----------|
| Invalid character | `"Unexpected token '%c'\n"` | 1 |
| Premature termination | `"Unexpected end of input\n"` | 1 |
| Malformed parentheses | `"Unexpected token '%c'\n"` | 1 |

### Error Detection Points
- Lexical analysis (invalid characters)
- Syntax analysis (malformed expressions)
- Parentheses matching validation

## Technical Glossary

| Term | Definition |
|------|------------|
| **AST** | Abstract Syntax Tree - hierarchical representation of expression structure |
| **Left-Associative** | Evaluation order where operators of equal precedence are processed left-to-right |
| **Recursive Descent** | Top-down parsing technique using recursive function calls |
| **Primary Expression** | Atomic expression unit (number or parenthetical group) |
| **Precedence Level** | Operator priority determining evaluation order |

## Implementation Workflow

```
Input Expression → Lexical Analysis → Syntax Analysis → AST Construction → Evaluation
                ↓                  ↓                ↓                    ↓
            Token Stream → Parse Tree → Abstract Tree → Result Value
```

### Parsing Phases

1. **Primary Parsing**: Identify base elements (numbers, parentheses)
2. **Multiplication Layer**: Handle high-precedence operations
3. **Addition Layer**: Process low-precedence operations
4. **Tree Assembly**: Construct final AST with proper associativity
5. **Validation**: Ensure syntactic correctness

The multi-layered approach ensures correct operator precedence while maintaining code modularity and error isolation.