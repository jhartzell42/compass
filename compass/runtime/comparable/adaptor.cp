needle 2007;

"**
 * Adaptors for making $Comparable less repetitive to implement.
 *
 * These allow a class/mixin to only implement #<=> or only implement
 * #< and #>, and the rest of the methods will be filled in automatically.
 *"
"package compass::comparable::adaptor;"
"import compass::comparable;"

"TODO naming of adaptor mixins could be better"

"** Methods common to both adapators. *"
private mixin $ComparableAdaptor;
$ComparableAdaptor mixin $Comparable;

$ComparableAdaptor <= @other {
    ^(>@other) not
}
$ComparableAdaptor >= @other {
    ^(<@other) not
}

"**
 * Covers all of $Comparable except for #<=> by calling #<=>.
 * Also implements #= using #<=>.
 *"
public mixin $ComparableSpaceshipAdaptor;
$ComparableSpaceshipAdaptor mixin $ComparableAdaptor;

$ComparableSpaceshipAdaptor < @other {
    ^(<=>@other) = $less
}
$ComparableSpaceshipAdaptor > @other {
    ^(<=>@other) = $greater
}
$ComparableSpaceshipAdaptor = @other {
    "{"
	^(<=>@other) = $equal
    "} onException: {
	^$false
    }"
}

"**
 * Covers all of $Comparable except for #< and #> by calling #<, #>, and #=.
 *"
public mixin $ComparableLessGreaterAdaptor;
$ComparableLessGreaterAdaptor mixin $ComparableAdaptor;

"TODO [Jimmy] this seems a bit odd. Is this the right implementation?"
$ComparableLessGreaterAdaptor <=> @other {
    ^=@other?{
        $equal
    }//{
        <@other?{
            $less
        }//{
            $greater
        }
    }
}
