needle 2007;

"TODO doc once approved"
"package compass::integer::sign;"
"import compass::comparable;"

public enum $Sign
    negative: ($negative)
    positive: ($positive)
    zero: ($zero);

public $ComparisonResult asSign {
    ^less:{$negative} greater:{$positive} equal:{$zero}
}
public $Sign asComparisonResult {
    ^negative:{$less} positive:{$greater} zero:{$equal}
}

public $Integer sign {
    ^(<=>0) asSign
}

"** Ask if I am positive. 
 *"
"TODO $Integer is way too deep in number for this"
public $Integer isPositive {
    ^sign=$positive
}
public $Integer isNegative {
    ^sign=$negative
}
