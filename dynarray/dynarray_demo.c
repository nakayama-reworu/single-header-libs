#include "dynarray.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


typedef enum {
    CONST,
    BIN_OP,
    UN_OP
} ItemType;


typedef struct {
    ItemType Type;
    union {
        char Op;
        int Value;
    };
} Item;


Item ItemConst(int value) {
    return (Item) {.Type = CONST, .Value = value};
}


Item ItemUnOp(char op) {
    return (Item) {.Type = UN_OP, .Op = op};
}


Item ItemBinOp(char op) {
    return (Item) {.Type = BIN_OP, .Op = op};
}


int EvalBinOp(char op, int arg1, int arg2) {
    switch (op) {
        case '+':
            return arg1 + arg2;
        case '-':
            return arg1 - arg2;
        case '*':
            return arg1 * arg2;
        case '/':
            return arg1 / arg2;
        default: {
            LOG_ERROR("Unsupported operation: %c", op);
            return 0;
        }
    }
}


int EvalUnOp(char op, int arg1) {
    if ('-' == op) {
        return -arg1;
    }

    LOG_ERROR("Unsupported operation: %c", op);
    return 0;
}


int RpnEval(Item *expression) {
    if (Array_IsEmpty(expression)) {
        return 0;
    }

    int *operands = Array_EmptyOfType(int);
    for (typeof(expression) cur = expression; cur != Array_End(expression); cur++) {
        switch (cur->Type) {
            case CONST:
                Array_Append(operands, cur->Value);
                break;
            case BIN_OP: {
                int op2 = Array_Pop(operands);
                int op1 = Array_Pop(operands);

                Array_Append(operands, EvalBinOp(cur->Op, op1, op2));

                break;
            }
            case UN_OP: {
                int op1 = Array_Pop(operands);

                Array_Append(operands, EvalUnOp(cur->Op, op1));

                break;
            }
        }
    }

    assert(Array_Size(operands) == 1);
    const int result = operands[0];

    Array_Free(operands, NULL);

    return result;
}


void ItemArrayPrint(Item *items, char *end) {
    if (NULL == end) {
        end = "\n";
    }

    printf("%s=[", nameof_identifier(items));
    for (typeof(items) item = items; item != Array_End(items); item++) {
        switch (item->Type) {
            case CONST:
                printf("%d%s", item->Value, item + 1 == Array_End(items) ? "" : " ");
                break;
            case BIN_OP:
            case UN_OP:
                printf("%c%s", item->Op, item + 1 == Array_End(items) ? "" : " ");
                break;
        }
    }

    printf("]");
    printf("%s", end);
}


int main() {
    // Create an empty array
    Item *expression = Array_EmptyOfType(Item);
    Array_ForEach(_, expression) {
        LOG_ERROR("This must never be reached");
    }

    ItemArrayPrint(expression, "\n");

    // Append single value -> [1]
    Array_Append(expression, ItemConst(1));
    ItemArrayPrint(expression, "\n");

    // Extend with several values -> [1 2 +]
    Array_ExtendWith(expression, ItemConst(2), ItemBinOp('+'));
    ItemArrayPrint(expression, "\n");

    // Temporary array with more items
    Item *minus_two = Array_Of(ItemConst(2), ItemUnOp('-'));

    // Extend the main array with another array -> [1 2 + 2 -]
    expression = Array_Extend(expression, minus_two);
    ItemArrayPrint(expression, "\n");
    Array_Free(minus_two, NULL);

    // Extend with array literal -> [1 2 + 2 - *]
    Array_ExtendWithValues(
            expression,
            ((Item[]) {ItemBinOp('*')}),
            1,
            sizeof(Item)
    );
    ItemArrayPrint(expression, "\n");

    // Temporary array with more items
    Item *plus_one = Array_OfType(Item, 2);
    plus_one[0] = ItemConst(1);
    plus_one[1] = ItemBinOp('+');

    // Extend the main array with another array -> [1 2 + 2 - * 1 +]
    expression = Array_Extend(expression, plus_one);
    ItemArrayPrint(expression, "\n");
    Array_Free(plus_one, NULL);

    ItemArrayPrint(expression, " -> ");
    printf("%d\n", RpnEval(expression));

    // Pop the last "+1" operation -> [1 2 + 2 - *]
    (void) Array_Pop(expression);
    (void) Array_Pop(expression);

    ItemArrayPrint(expression, " -> ");
    printf("%d\n", RpnEval(expression));

    Array_Free(expression, NULL);

    return EXIT_SUCCESS;
}
