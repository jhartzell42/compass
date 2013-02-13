needle 2007;

"**
 * General implementation of integer exponentiation.
 *"
"package compass::number::integer!!exponentiation;"

"TODO style guide on spacing?"
public $Integer ** @exponent {
    @exponent isEven?{
        ^(* self) ** (@exponent \ 2)
    }//{
        ^* (** (@exponent - 1))
    }
}
