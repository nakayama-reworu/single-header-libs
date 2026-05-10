#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


typedef enum {
    RPN_CONST,
    RPN_BIN_OP,
    RPN_UN_OP
} RpnItemType;


typedef struct {
    RpnItemType Type;
    union {
        char Op;
        int Value;
    };
} RpnItem;


RpnItem RpnItemConst(int value) {
    return (RpnItem) {.Type = RPN_CONST, .Value = value};
}


RpnItem RpnItemUnOp(char op) {
    return (RpnItem) {.Type = RPN_UN_OP, .Op = op};
}


RpnItem RpnItemBinOp(char op) {
    return (RpnItem) {.Type = RPN_BIN_OP, .Op = op};
}


int RpnEvalBinOp(char op, int arg1, int arg2) {
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


int RpnEvalUnOp(char op, int arg1) {
    if ('-' == op) {
        return -arg1;
    }

    LOG_ERROR("Unsupported operation: %c", op);
    return 0;
}


int RpnEval(RpnItem *expression) {
    if (Array_IsEmpty(expression)) {
        return 0;
    }

    int *operands = Array_EmptyOfType(int);
    for (typeof(expression) cur = expression; cur != Array_End(expression); cur++) {
        switch (cur->Type) {
            case RPN_CONST:
                Array_Append(operands, cur->Value);
                break;
            case RPN_BIN_OP: {
                int op2 = Array_Pop(operands);
                int op1 = Array_Pop(operands);

                Array_Append(operands, RpnEvalBinOp(cur->Op, op1, op2));

                break;
            }
            case RPN_UN_OP: {
                int op1 = Array_Pop(operands);

                Array_Append(operands, RpnEvalUnOp(cur->Op, op1));

                break;
            }
        }
    }

    assert(Array_Size(operands) == 1);
    const int result = operands[0];

    Array_Free(operands, NULL);

    return result;
}


void ItemArrayPrint(RpnItem *items, char *end) {
    if (NULL == end) {
        end = "\n";
    }

    printf("%s=[", nameof_identifier(items));
    for (typeof(items) item = items; item != Array_End(items); item++) {
        switch (item->Type) {
            case RPN_CONST:
                printf("%d%s", item->Value, item + 1 == Array_End(items) ? "" : " ");
                break;
            case RPN_BIN_OP:
            case RPN_UN_OP:
                printf("%c%s", item->Op, item + 1 == Array_End(items) ? "" : " ");
                break;
        }
    }

    printf("]");
    printf("%s", end);
}


int main() {
    // Create an empty array
    RpnItem *expression = Array_EmptyOfType(RpnItem);
    Array_ForEach(_, expression) {
        LOG_ERROR("This must never be reached");
    }

    ItemArrayPrint(expression, "\n");

    // Append single value -> [1]
    Array_Append(expression, RpnItemConst(1));
    ItemArrayPrint(expression, "\n");

    // Extend with several values -> [1 2 +]
    Array_ExtendWith(expression, RpnItemConst(2), RpnItemBinOp('+'));
    ItemArrayPrint(expression, "\n");

    // Temporary array with more items
    RpnItem *minus_two = Array_Of(RpnItemConst(2), RpnItemUnOp('-'));

    // Extend the main array with another array -> [1 2 + 2 -]
    expression = Array_Extend(expression, minus_two);
    ItemArrayPrint(expression, "\n");
    Array_Free(minus_two, NULL);

    // Extend with array literal -> [1 2 + 2 - *]
    Array_ExtendWithValues(
            expression,
            ((RpnItem[]) {RpnItemBinOp('*')}),
            1,
            sizeof(RpnItem)
    );
    ItemArrayPrint(expression, "\n");

    // Temporary array with more items
    RpnItem *plus_one = Array_OfType(RpnItem, 2);
    plus_one[0] = RpnItemConst(1);
    plus_one[1] = RpnItemBinOp('+');

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
