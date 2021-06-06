struct linkedList{
    int data;
    linkedList * next;
};

linkedList * createLinkedList(int data){
    linkedList *  l = (struct linkedList*)malloc(sizeof(struct linkedList));

    l->data=data;
    l->next=NULL;

    return l;
}


void freeLinkedList(linkedList *head){
    if (head==NULL){
        return;
    }else {
        linkedList*aux=head;
        head=head->next;
        free(aux);
        freeLinkedList(head);
    }
}

void printLinkedList(linkedList* head){
    while(head!=NULL){
        printf("%i->",head->data);
        head=head->next;
    }
    printf("NULL\n");
}

void printLinkedList2(linkedList* head){
    while(head!=NULL){
        printf("%i ",head->data);
        head=head->next;
    }
    printf("NULL\n");
}

linkedList* concatenateLinkedList(linkedList* l1,linkedList* l2){
    if (l1==NULL && l2==NULL){
        return NULL;
    }else if(l1==NULL){
        return l2;
    }else if(l2==NULL){
        return l1;
    }else{
        if (l1->next==NULL){
            l1->next=l2;
            return l1;
        }
        linkedList * aux=l1;
        while (aux->next!=NULL){
            aux=aux->next;
        }
        aux->next=l2;
        return l1;
    }
}