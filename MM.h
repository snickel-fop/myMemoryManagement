/* MemoryManagement von Philipp Duspiwa und Benjamin Zastrow
*  Version 1.0 ("Kangaroo")
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Block {
    char *startAdr;
    int size; //in Byte
    int free; //1: frei, 0: belegt
    char name[50];
};

struct LE {
    struct Block *content;
    struct LE* nxt;
    struct LE* prev;
};


char *heap = NULL;
int heapSize = 0;
int strat = 0;
struct LE* list = NULL;
struct LE* NextFitPtr = NULL;

void setListtoBeginning() {
    while(list->prev != NULL) {
        list = list->prev;
    }
}

struct LE* shrinkElement(struct LE *element, int newSize) { //teilt Element in zwei Subelemente und gibt das neue Element zurück (bereits in Liste eingefügt)
    if(element == NULL || newSize <= 0) {
        return NULL;
    }
    struct Block *tmpBlock = malloc(sizeof(struct Block));
    struct LE *tmpNode = malloc(sizeof(struct LE));
    if(tmpBlock == NULL || tmpNode == NULL) {
        return NULL;
    }
    tmpNode->content = tmpBlock;
    tmpNode->content->size = newSize;
    element->content->size -= newSize;
    tmpNode->content->startAdr = element->content->startAdr;
    element->content->startAdr += newSize;
    tmpNode->content->free = 1;

    //tmpNode in Liste integrieren
    if(element->prev != NULL) {
        element->prev->nxt = tmpNode;
        tmpNode->prev = element->prev;
    }
    else {
        tmpNode->prev = NULL;
    }

    tmpNode->nxt = element;
    element->prev = tmpNode;

    return tmpNode;
}

void mminit(int size, int strategy) { //MemoryManagement (Heap und Verwaltungsliste) initialisieren
    heap = (char*) malloc(size*sizeof(char));
    if(heap == NULL) {
        printf("Heap konnte nicht initialisiert werden!\n");
        return;
    }
    strat = strategy;
    list = (struct LE*) malloc(sizeof(struct LE));
    struct Block *tmpBlock = (struct Block*) malloc(sizeof (struct Block));
    list->content = tmpBlock;
    heapSize = size;
    if(list != NULL) {
        list->content->startAdr = &heap[0];
        list->content->size = size;
        list->content->free = 1;
        strcpy(list->content->name, "-");
        list->nxt = NULL;
        list->prev = NULL;
    }
}

void mmterm() { //MemoryManagement beenden (Heap freigeben und Liste "exterminieren")
    struct LE *tmp = list;
    while(tmp != NULL) {
        if(tmp->prev != NULL) {
            free(tmp->prev);
        }

        if(tmp->nxt != NULL) {
            tmp = tmp->nxt;
        }
        else {
            free(tmp);
            break;
        }
    }

    free(heap);
}

char* mmmalloc(int size, char *name) { //Speicher reservieren mittels angegebener Strategie (bei init festgelegt), Groesse und einem Namen
    switch (strat) {
        case 1: {
            //FirstFit
            struct LE *tmp = list;
            while(tmp != NULL) {
                if(tmp->content->free == 1) {
                    if(tmp->content->size == size) {
                        tmp->content->free = 0;
                        strcpy(tmp->content->name, name);
                        return tmp->content->startAdr;
                    }
                    else if(tmp->content->size > size) {
                        struct LE *ret = shrinkElement(tmp, size);
                        strcpy(ret->content->name, name);
                        ret->content->free = 0;
                        setListtoBeginning();
                        return ret->content->startAdr;
                    }
                }
                if(tmp->nxt != NULL) {
                    tmp = tmp->nxt;
                }
                else {
                    printf("Fehler: Angeforderte Groesse zu gross!\n");
                    break;
                }
            }
            return NULL;
        }
        case 2: {
            //NextFit

            if(NextFitPtr == NULL) {
                NextFitPtr = list;
            }

            struct LE *tmp = list;

            int counter = 0;
            while(tmp != NULL) { //Listenelemente zählen
                counter++;
                if(tmp->nxt == NULL) {
                    break;
                }
                else {
                    tmp = tmp->nxt;
                }
            }


            for(int i = 0; i < counter; ++i) {
                if(NextFitPtr->content->free == 1) {
                    if(NextFitPtr->content->size == size) {
                        NextFitPtr->content->free = 0;
                        strcpy(NextFitPtr->content->name, name);
                        return NextFitPtr->content->startAdr;
                    }
                    else if(NextFitPtr->content->size > size) {
                        struct LE *ret = shrinkElement(NextFitPtr, size);
                        if(ret == NULL) {
                            return NULL;
                        }
                        strcpy(ret->content->name, name);
                        ret->content->free = 0;
                        setListtoBeginning();
                        return ret->content->startAdr;
                    }
                }
                if(NextFitPtr->nxt == NULL) {
                    NextFitPtr = list;

                }
                else {
                    NextFitPtr = NextFitPtr->nxt;
                }
            }
            return NULL;
        }
        case 3: {
            //BestFit
            int bestsize = heapSize + 1;
            struct LE *besthit = NULL;

            struct LE *tmp = list;
            while(tmp != NULL) {
                if(tmp->content->free == 1) {
                    if(tmp->content->size >= size) {
                        if(tmp->content->size < bestsize) {
                            bestsize = tmp->content->size;
                            besthit = tmp;
                        }
                    }
                }
                if(tmp->nxt != NULL) {
                    tmp = tmp->nxt;
                }
                else {
                    break;
                }
            }

            struct LE *ret = shrinkElement(besthit, size);
            if(ret == NULL) {
                setListtoBeginning();
                return NULL;
            }
            else {
                ret->content->free = 0;
                strcpy(ret->content->name, name);
                setListtoBeginning();
                return ret->content->startAdr;
            }
            break;
        }
        case 4: {
            //WorstFit
            int bestsize = 0;
            struct LE *besthit = NULL;

            struct LE *tmp = list;
            while(tmp != NULL) {
                if(tmp->content->free == 1) {
                    if(tmp->content->size >= size) {
                        if(tmp->content->size > bestsize) {
                            bestsize = tmp->content->size;
                            besthit = tmp;
                        }
                    }
                }
                if(tmp->nxt != NULL) {
                    tmp = tmp->nxt;
                }
                else {
                    break;
                }
            }

            struct LE *ret = shrinkElement(besthit, size);
            if(ret == NULL) {
                setListtoBeginning();
                return NULL;
            }
            else {
                ret->content->free = 0;
                strcpy(ret->content->name, name);
                setListtoBeginning();
                return ret->content->startAdr;
            }       
            break;
        }
        default: {
            printf("Keine gueltige Strategie gewaehlt!\n");
            break;
        }
    }

    return NULL;
}

void mmfree(char *startAdr) { //reserviertes Element freigeben
    int status = -1; //0: davor/danach belegt, 1: davor frei/danach belegt, 2: davor belegt/danach frei, 3: davor und danach frei
    //Liste durchgehen, startAdr finden, ist Block davor/danach frei?
    struct LE *tmp = list;
    int tempCounter = 0;
    while(tmp != NULL) {

        if(tmp->content->startAdr == startAdr) { //gotcha
            if(tmp->prev != NULL && tmp->prev->content->free == 1) { // h8lp, Fehlerquelle
                if(tmp->nxt != NULL && tmp->nxt->content->free == 1) {
                    status = 3;
                    break;
                }
                else if(tmp->nxt == NULL || (tmp->nxt != NULL && tmp->nxt->content->free == 0)) {
                    status = 1;
                    break;
                }
            }
            else if(tmp->nxt != NULL && tmp->nxt->content->free == 1) {
                if(tmp->prev != NULL && tmp->prev->content->free == 0) {
                    status = 2;
                    break;
                }
            }
            if(tmp->nxt == NULL) {
                tempCounter++;
            }
            else if(tmp->nxt->content->free == 0) {
                tempCounter++;
            }
            if(tmp->prev == NULL) {
                tempCounter++;
            }
            else if(tmp->prev->content->free == 0) {
                tempCounter++;
            }

            if(tempCounter == 2) {
                status = 0;
                break;
            }
        }

        if(tmp->nxt != NULL) {
            tmp = tmp->nxt;
        }
        else {
            break;
        }
    }
    //wenn keiner frei: free einfach auf 1
    if(status == 0) {
        tmp->content->free = 1;
        strcpy(tmp->content->name, "-");

        return;
    }
    //wenn einer/zwei frei: vereinen (davor/danach/beides)
    else if(status == 1) {
        if(tmp->nxt == NULL || tmp->prev == NULL) {return;}
        NextFitPtr = list;
        tmp->prev->nxt = tmp->nxt;
        tmp->nxt->prev = tmp->prev;
        tmp->prev->content->size += tmp->content->size;
        tmp->prev->content->free = 1;
        free(tmp);

    }
    else if(status == 2) {
        if(tmp->nxt == NULL || tmp->prev == NULL) {return;}
        NextFitPtr = list;
        tmp->content->startAdr = tmp->prev->content->startAdr;
        tmp->nxt->prev = tmp->prev;
        tmp->prev->nxt = tmp->nxt;
        tmp->nxt->content->size += tmp->content->size;
        tmp->nxt->content->free = 1;
        free(tmp);
    }
    else if(status == 3) {
        if(tmp->nxt == NULL || tmp->prev == NULL) {return;}
        NextFitPtr = list;
        tmp->prev->nxt = tmp->nxt->nxt;
        if(tmp->nxt->nxt != NULL) {
            tmp->nxt->nxt->prev = tmp->prev;
        }
        tmp->prev->content->size += tmp->content->size + tmp->nxt->content->size;
        free(tmp->nxt);
        free(tmp);
    }
}

void mmrename(char *startAdr, char *name) { //Liste durchgehen: startAdr finden, Name auf name ändern
    struct LE *tmp = list;
    while(tmp != NULL) {

        if(tmp->content->startAdr == startAdr) {
            strcpy(tmp->content->name, name);
            break;
        }

        if(tmp->nxt != NULL) {
            tmp = tmp->nxt;
        }
        else {
            break;
        }
    }
}

void mmdump() { //Liste durchgehen, Eigenschaften ausgeben
    printf("Heapsize: %d\n", heapSize);
    printf("Startadresse\tGroesse\tStatus\tName\n");
    int freeCounter = 0, occCounter = 0, freeBytes = 0, occBytes = 0;
    struct LE *tmp = list;
    while(tmp != NULL) {

        if(tmp->content->free == 0) {
            printf("%p\t%d\t%s\t%s\n", (void*) tmp->content->startAdr, tmp->content->size, "belegt", tmp->content->name);
            occCounter++;
            occBytes += tmp->content->size;
        }
        else {
            printf("%p\t%d\t%s\t%s\n", (void*) tmp->content->startAdr, tmp->content->size, "frei", tmp->content->name);
            freeCounter++;
            freeBytes += tmp->content->size;
        }

        if(tmp->nxt != NULL) {
            tmp = tmp->nxt;
        }
        else {
            break;
        }
    }
    printf("Belegte Bloecke:\t%d (%d Bytes / %.1f Prozent)\n", occCounter, occBytes, ((float) occBytes / (float) heapSize) * 100);
    printf("Freie Bloecke:\t%d (%d Bytes / %.1f Prozent)\n", freeCounter, freeBytes, ((float) freeBytes / (float) heapSize) * 100);
}
