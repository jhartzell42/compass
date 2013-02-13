needle 2007;

"TODO doc"
"package compass::collection::fixedSize;"

public mixin $FixedSizeCollection;
$FixedSizeCollection mixin $Collection;

"**
 * The collection cannot perform the operation because it cannot change
 * in size.
 *"
public exception $FixedSizeException ($UnsupportedOperationException)
    #onFixedSize:;

$FixedSizeCollection delete: _ {
    $FixedSizeException()
}
$FixedSizeCollection add: _ {
    $FixedSizeException()
}
$FixedSizeCollection remove: _ {
    $FixedSizeException()
}
$FixedSizeCollection pop {
    $FixedSizeException()
}
$FixedSizeCollection push: _ {
    $FixedSizeException()
}
$FixedSizeCollection dequeue {
    $FixedSizeException()
}
$FixedSizeCollection enqueue: _ {
    $FixedSizeException()
}
$FixedSizeCollection clear {
    $FixedSizeException assert: size=0
}
$FixedSizeCollection retainAll: _ {
    $FixedSizeException()
}
