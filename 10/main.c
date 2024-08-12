#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 創建節點
typedef struct binomialnode
{
    int degree;
    int priority;
    int JobId;
    struct binomialnode *parent;
    struct binomialnode *child;
    struct binomialnode *sibling;
} BinomialNode;

typedef struct binomialheap
{
    BinomialNode *head;
} BinomialHeap;

BinomialNode *createnode(int jobid, int priority) // 初始化一個要插入的數字
{
    BinomialNode *Newnode = (BinomialNode *)malloc(sizeof(BinomialNode));
    Newnode->priority = priority;
    Newnode->JobId = jobid;
    Newnode->degree = 0;
    Newnode->parent = Newnode->child = Newnode->sibling = NULL;
    return Newnode;
}

// 將要合併的兩個heap從小的degree排到大
BinomialNode *mergeRootLists(BinomialNode *head1, BinomialNode *head2)
{
    if (!head1) // 當某printer插入第一個數字直接return head2
    {
        return head2;
    }
    if (!head2)
    {
        return head1;
    }
    // 比較誰的degree比較高
    BinomialNode dummy;
    BinomialNode *current = &dummy;
    while (head1 && head2)
    {
        if (head1->degree <= head2->degree)
        {
            current->sibling = head1;
            head1 = head1->sibling;
        }
        else
        {
            current->sibling = head2;
            head2 = head2->sibling;
        }
        current = current->sibling;
    }
    current->sibling = (head1 ? head1 : head2);
    return dummy.sibling;
}

void linkBinomialTrees(BinomialNode *parent, BinomialNode *child)
{
    child->parent = parent;
    child->sibling = parent->child; // 將當前的child鏈接為parent的最左側子節點
    parent->child = child;
    parent->degree += 1;
}

// 兩個heap合在一起
BinomialHeap *mergeBinomialHeaps(BinomialHeap *heap1, BinomialHeap *heap2)
{
    // Make an empty binomialHeap
    BinomialHeap *result = (BinomialHeap *)malloc(sizeof(BinomialHeap));
    result->head = NULL;
    BinomialNode *mergedHeads = mergeRootLists(heap1->head, heap2->head);
    BinomialNode *prev = NULL;
    BinomialNode *current = mergedHeads;
    BinomialNode *next = NULL;
    if (current)
    {
        next = current->sibling;
    }
    result->head = mergedHeads;
    while (next != NULL)
    {
        if (current->degree != next->degree || (next->sibling && next->sibling->degree == current->degree))
        {
            prev = current;
            current = next;
        }
        else
        {
            if (current->priority >= next->priority)
            {
                current->sibling = next->sibling;
                linkBinomialTrees(current, next);
            }
            else
            {
                if (!prev)
                {
                    mergedHeads = next;
                    result->head = mergedHeads;
                }
                else
                {
                    prev->sibling = next;
                }
                linkBinomialTrees(next, current);
                current = next;
            }
        }
        next = current->sibling;
    }
    return result;
}

void add(BinomialHeap *heap, int jobid, int priority, int printer_id, int count)
{
    BinomialHeap *newNodeHeap = (BinomialHeap *)malloc(sizeof(BinomialHeap));
    newNodeHeap->head = createnode(jobid, priority);

    BinomialHeap *mergeheap = mergeBinomialHeaps(heap, newNodeHeap);

    heap->head = mergeheap->head;

    printf("%d jobs waiting on printer %d\n", count, printer_id);
}

// 反轉子節點
BinomialNode *reverseSiblingList(BinomialNode *child)
{
    BinomialNode *prev = NULL;
    BinomialNode *current = child;
    BinomialNode *next = NULL;
    while (current)
    {
        next = current->sibling;
        current->sibling = prev;
        prev = current;
        current = next;
    }
    return prev;
}

void print(BinomialHeap *heap)
{
    if (!heap->head)
    {
        printf("no documents in queue\n");
    }
    else
    {
        BinomialNode *MaxPre = NULL;
        BinomialNode *Pre = NULL;
        BinomialNode *MaxNode = heap->head;
        BinomialNode *current = heap->head;
        int MaxPriority = MaxNode->priority;
        while (current) // 尋找最大priority的node
        {
            if (current->priority > MaxPriority)
            {
                MaxPre = Pre;
                MaxNode = current;
                MaxPriority = current->priority;
            }
            Pre = current;
            current = current->sibling;
        }
        printf("%d printed\n", MaxNode->JobId);
        BinomialNode *child = MaxNode->child;
        while (child != NULL)
        {
            child->parent = NULL;
            child = child->sibling;
        }
        if (MaxPre) // 不是頭節點是MAX
        {
            BinomialHeap *tempHeapForHeadToMaxPre = (BinomialHeap *)malloc(sizeof(BinomialHeap));
            BinomialHeap *tempHeapForMaxNodeChild = (BinomialHeap *)malloc(sizeof(BinomialHeap));

            tempHeapForHeadToMaxPre->head = heap->head;
            MaxPre->sibling = NULL;

            tempHeapForMaxNodeChild->head = reverseSiblingList(MaxNode->child);

            BinomialHeap *firstMergedHeap = mergeBinomialHeaps(tempHeapForHeadToMaxPre, tempHeapForMaxNodeChild);

            BinomialHeap *tempHeapForMaxNodeSibling = (BinomialHeap *)malloc(sizeof(BinomialHeap));
            tempHeapForMaxNodeSibling->head = MaxNode->sibling;

            BinomialHeap *finalMergedHeap = mergeBinomialHeaps(firstMergedHeap, tempHeapForMaxNodeSibling);

            heap->head = finalMergedHeap->head;
        }
        else
        {
            if (MaxNode->child)
            {
                if (MaxNode->sibling)
                {
                    BinomialHeap *tempHeap1 = (BinomialHeap *)malloc(sizeof(BinomialHeap));
                    BinomialHeap *tempHeap2 = (BinomialHeap *)malloc(sizeof(BinomialHeap));
                    tempHeap1->head = reverseSiblingList(MaxNode->child);
                    tempHeap2->head = MaxNode->sibling;
                    BinomialHeap *mergedHeap = mergeBinomialHeaps(tempHeap1, tempHeap2);
                    heap->head = mergedHeap->head;
                }
                else
                {
                    heap->head = MaxNode->child; // 直接sibling當作新的head
                }
            }
            else
            {
                heap->head = MaxNode->sibling;
            }
        }
    }
}

void move(BinomialHeap *heap1, BinomialHeap *heap2)
{
    BinomialHeap *mergeHeap = mergeBinomialHeaps(heap1, heap2);
    heap2->head = mergeHeap->head;
    heap1->head = NULL;
}

int main()
{
    int N, M, operator, printer_id, printer_id2;
    long long job_id, priority;

    scanf("%d %d", &N, &M);
    int count[N];
    memset(count, 0, sizeof(count));

    BinomialHeap *printers = (BinomialHeap *)calloc(N, sizeof(BinomialHeap));

    for (int i = 0; i < M; i++)
    {
        scanf("%d", &operator);
        switch (operator)
        {
        case 1:
            scanf("%lld %lld %d", &job_id, &priority, &printer_id);
            count[printer_id - 1] += 1;
            add(&printers[printer_id - 1], job_id, priority, printer_id, count[printer_id - 1]);
            break;
        case 2:
            scanf("%d", &printer_id);
            if (printers[printer_id - 1].head)
            {
                count[printer_id - 1] -= 1;
            }
            print(&printers[printer_id - 1]);
            break;
        case 3:
            scanf("%d %d", &printer_id, &printer_id2);
            count[printer_id2 - 1] += count[printer_id - 1];
            count[printer_id - 1] = 0;
            move(&printers[printer_id - 1], &printers[printer_id2 - 1]);
            printf("%d jobs waiting on printer %d after moving\n", count[printer_id2 - 1], printer_id2);
            break;
        }
    }
    return 0;
}