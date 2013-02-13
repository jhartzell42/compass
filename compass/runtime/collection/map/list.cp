needle 2007;

"TODO doc"
"** Very simple implementation of map using a list. Nearly useless. *"
"package compass::collection::map::list;"

public class $ListMap
    !keyEquivalenceRelation,
    !valueEquivalenceRelation,
    "** A $List of $MapEntry objects. *"
    !list;
$ListMap mixin $Map;

public $ListMap constructor
        newWithKeyEquivalenceRelation: !keyEquivalenceRelation
        valueEquivalenceRelation: !valueEquivalenceRelation {
    !list := $List new; "TODO Proper way to construct a list?"
    "TODO should be a linked list, not an array list..."
}

private $ListMap list (0,ro) {
    ^!list
}
public $ListMap size (7,ro) {
    ^list size
}
public $ListMap equivalenceRelation (1,ro) {
    ^!valueEquivalenceRelation
}
public $ListMap keyEquivalenceRelation (1,ro) {
    ^!keyEquivalenceRelation
}
public $ListMap serialLoop: @blk (7,ro) {
    list serialLoop: { @el -- @blk(@el value, @el key) }
}
private $ListMap indexOf: @key (0,ro) {
    ^{list indexOfElementMatching: { @entry -- 
        isKey: @entry key equalTo: @key
    }} onNoMatch: {
        $InvalidLookupException(@key) "TODO this call does not exist yet"
    }
}
public $ListMap [ @key ] (7,ro) {
    ^list[indexOf: @key] value
}
public $ListMap :[ @key ]= @value (7,rw) {
    "TODO This is slow on linked lists, only works fast on array lists"
    @newEntry := $SimpleMapEntry newWithKey: @key value: @value;
    { list :[indexOf: @key]= @newEntry } onInvalidLookup: {
        list add: @newEntry
    }
}
