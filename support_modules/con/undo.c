/*
  Undo Stack
*/

#include <stdlib.h>
#include <string.h>
#include "undo.h"

#define UV_SIZE sizeof(UndoValue)

/**
 * Initialize UndoStack structure.
 *
 * \param byteLimit     Maximum byte size of undo history.
 */
int undo_init(UndoStack* us, uint32_t byteLimit)
{
    const size_t initLimit = 1024*4;
    size_t bytes = byteLimit;

    us->used = 0;
    us->pos = 0;
    us->byteLimit = byteLimit;

    if (bytes > initLimit)
        bytes = initLimit;
    us->avail = bytes / UV_SIZE;
    us->stack = (UndoValue*) malloc(us->avail * UV_SIZE);
    if (! us->stack)
        return 0;
    us->stack[0].u = Undo_Term;
    return 1;
}

void undo_free(UndoStack* us)
{
    free(us->stack);
    us->stack = NULL;
}

void undo_clear(UndoStack* us)
{
    us->used = 0;
    us->pos = 0;
    us->stack[0].u = Undo_Term;
}

static void undo_discardHistory(UndoStack* us)
{
    const UndoValue* ep = us->stack;
    const size_t eraseLimit = 1024*8;
    size_t total, stepLen;
    size_t eraseLen = us->byteLimit / 4;

    if (eraseLen > eraseLimit)
        eraseLen = eraseLimit;
    eraseLen /= UV_SIZE;

    // Walk steps until eraseLen is reached.
    total = 0;
    while (total < eraseLen) {
        stepLen = ep->op.skipNext;
        ep += stepLen;
        total += stepLen;
    }

    // TODO: Allow user to process removed steps.
    // us->eraseCallback(us->stack, total, us->user);
    // An alternative to a callback is to abort undo_record() and have the
    // user call an undo_erase() function.

    us->used -= total;
    memcpy(us->stack, ep, us->used * UV_SIZE);
    if (us->pos <= total)
        us->pos = 0;
    else
        us->pos -= total;
}

/**
 * Add a new step to the undo history.
 *
 * The step is added at the current position.  Any history after the current
 * position is discarded.
 *
 * If the history size grows beyond the byteLimit specified with undo_init()
 * then some previous history will be discarded.
 *
 * \param opcode    User identifer of undo step. Zero (Undo_Term) is reserved.
 * \param data      Data for undo step.
 * \param values    Number of data items.  Must be less than 255.
 */
void undo_record(UndoStack* us, uint16_t opcode, const UndoValue* data,
                 int values)
{
    UndoValue* top;
    int stepLen = values + 1;

    // Adding one for the Undo_Term.
    if ((us->used + stepLen + 1) > us->avail) {
        size_t count = us->avail * 2;
        size_t bytes = (count * UV_SIZE) + UV_SIZE;

        if (bytes > us->byteLimit)
            undo_discardHistory(us);    // Assuming at least bytes remain.

        us->stack = (UndoValue*) realloc(us->stack, bytes);
        us->avail = count;
    }

    top = us->stack + us->pos;
    top->op.code = opcode;
    top->op.skipNext = stepLen;
    memcpy(++top, data, values * UV_SIZE);
    us->pos += stepLen;
    us->used = us->pos;

    // New terminator.
    top += values;
    top->u = Undo_Term;         // Sets both op.code & op.skipNext.
    top->op.skipPrev = stepLen;
}

/**
 * Move back one step in undo history.
 *
 * \param step  The pointer to the previous step is written here.
 *              If Undo_AtEnd is returned this value will be NULL.
 *
 * \return UndoResult mask.
 */
int undo_stepBack(UndoStack* us, const UndoValue** step)
{
    UndoValue* top;
    int stepLen;
    int adv;

    if (! us->pos) {
        *step = NULL;
        return Undo_AtEnd;
    }

    adv = Undo_Advanced;
    if (us->pos == us->used)
        adv |= Undo_AdvancedFromStart;

    top = us->stack + us->pos;
    stepLen = top->op.skipPrev;
    *step = top - stepLen;
    us->pos -= stepLen;

    if (us->pos == 0)
        adv |= Undo_AdvancedToEnd;
    return adv;
}

/**
 * Move forward one step in undo history.
 *
 * \param step  The pointer to the next step is written here.
 *              If Undo_AtEnd is returned this value will be NULL.
 *
 * \return UndoResult mask.
 */
int undo_stepForward(UndoStack* us, const UndoValue** step)
{
    UndoValue* top;
    int adv;

    if (us->pos == us->used) {
        *step = NULL;
        return Undo_AtEnd;
    }

    adv = Undo_Advanced;
    if (us->pos == 0)
        adv |= Undo_AdvancedFromStart;

    top = us->stack + us->pos;
    *step = top;
    us->pos += top->op.skipNext;

    if (us->pos == us->used)
        adv |= Undo_AdvancedToEnd;
    return adv;
}
