from dumper import *

"""
    Um diese Funktionen zu debuggen:
    Stacktraces oder Fehlermeldungen gibt es nicht. Dafür braucht man printf-debugging.
    $ touch /tmp/log.txt
    $ tail -f /tmp/log.txt
    Terminal auf den zweiten Bildschirm schieben
    Bei print() IMMER darauf achten, dass file=open("/tmp/log.txt", "a") gesetzt ist.
    In QtCreator rumpfuschen, Ergebnisse live und in Echtzeit auf dem zweiten Bildschirm verfolgen.

    Typische Probleme
    P: Mein Output kommt nicht in der Konsole an.
    A: Du hast file=open("/tmp/log.txt", "a") vergessen!
    P: Der Output kommt trotz file=open... nicht in der Konsole an.
    A: Dann wurde die Stelle nicht ausgeführt, weil das Script vorher abgekackt ist.
"""

def qdump__array19__Array(d, value):
    inner_type = value.type[0]
    count = value.type[1]
    d.putItemCount(count)
    if d.isExpanded():
        d.putPlotData(value.address(), count, inner_type)

def qdump__array19__SliceOf(d, value):
    inner_type = value.type[0]
    data = value['m_data'].pointer()
    count = value['m_count'].integer()
    d.putItemCount(count)
    if count > 0:
        d.checkPointer(data)
    d.putPlotData(data, count, inner_type)

def qdump__array19__MoveSliceOf(d, value):
    inner_type = value.type[0]
    data = value['m_data'].pointer()
    count = value['m_count'].integer()
    d.putItemCount(count)
    if count > 0:
        d.checkPointer(data)
    d.putPlotData(data, count, inner_type)

def qdump__array19__DynamicArrayOf(d, value):
    inner_type = value.type[0]
    data = value['m_pointer'].pointer()
    count = value['m_count'].integer()

    d.putItemCount(count)
    d.putNumChild(count + 1)
    if count > 0:
        d.checkPointer(data)
    if d.isExpanded():
        with Children(d):
            for i in range(count):
                d.putSubItem(i, d.createValue(data + i * inner_type.size(), inner_type))
            d.putSubItem('capacity', value["m_capacity"])

def qdump__variant19__Variant(d, value):
    whichValue = value["indexed"]["which"].integer()

    prettyType = value.type.templateArguments()[whichValue].name

    d.putValue(prettyType)
    d.putNumChild(1)

    if d.isExpanded():
        with Children(d):
            d.putSubItem(0, d.createValue(value["indexed"]["storage"].address(), value.type[whichValue]))

def qdump__tuple19__Tuple(d, value):
    allTypes = value.type.templateArguments()
    numTypes = len(allTypes)

    d.putNumChild(numTypes)
    indexed = value['indexed']

    if d.isExpanded():
        with Children(d):
            for i in range(numTypes):
                base = indexed.members(True)[i]
                d.putSubItem(i, base['v'])

def accessBitsetIndex(d, bitset, index):
    data = bitset['m']
    bitsPerWord = int(bitset['BitsPerT'])
    currentWord = int(d.createValue(data.address() + index // bitsPerWord, data.type))
    return currentWord >> (index % bitsPerWord) & 1

def qdump__partial19__Bitset(d, value):
    bitCount = value.type.templateArguments()[0]

    d.putValue("Hello %d" % bitCount)

    d.putNumChild(bitCount)
    if d.isExpanded():
        with Children(d):
            for i in range(bitCount):
                with SubItem(d, i):
                    d.putValue(accessBitsetIndex(d, value, i))

def qdump__partial19__Partial(d, value):
    allTypes = value.type.templateArguments()
    numTypes = len(allTypes)

    alignments = value["alignof_ts"]
    sizes = value["sizeof_ts"]
    m_data = int(value["m_pointer"])
    m_bits = value["m_bits"]

    existingTypes = [i for i in range(numTypes) if accessBitsetIndex(d, m_bits, i)]
    d.putNumChild(len(existingTypes))

    if d.isExpanded():
        with Children(d):
            offset = 0
            for i in existingTypes:
                alignment = int(alignments.nativeValue[i])
                if offset * alignment != 0: # algorithm copied from partial19::alignOffset
                    offset = (((offset - 1) // alignment) + 1) * alignment
                d.putSubItem(i, d.createValue(m_data + offset, allTypes[i]))
                offset += int(sizes.nativeValue[i])
