needle 2007;

"TODO doc"
"package compass::collection::map::entry;"

public mixin $MapEntry;
$MapEntry mixin $Immutable; " TODO Are all map entries immutable? "

public $MapEntry abstract #key;
public $MapEntry abstract #value;

public class $SimpleMapEntry
    !key,
    !value,
;
$SimpleMapEntry mixin $MapEntry;
public $SimpleMapEntry newWithKey: !key value: !value;

$SimpleMapEntry key {
    ^!key
}

$SimpleMapEntry value {
    ^!value
}

public class $SimpleSetEntry
    !value,
;
$SimpleSetEntry mixin $MapEntry;
public $SimpleSetEntry newWithValue: !value;

$SimpleSetEntry value {
    ^!value
}
$SimpleSetEntry alias #key #value;
