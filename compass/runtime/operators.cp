needle 2007;

"**
 * Operators with generalized definitions with <@link=$Object> as the lowest
 * common denominator.
 *
 * These operators have a similar feel to them across different types, and
 * therefore are defined here as applying to all types with very general
 * semantics.
 *
 * However, they are not defined for all types, and there is no default
 * definition. So it is inappropriate to put them along with $Object in the
 * <@link=compass::object> package.
 *
 * @author Daniel Perelman (dap56@cornell.edu)
 * @since needle 2007
 *"
"package compass::operators;"

"How strict should these definitions be: Strict where reasonable seems
    good. If one would expect a message to be restrict, it should be here."

"**
 * Looping operator.
 *
 * The receiver determines how many times the loop should run and what
 * arguments (if any) are given to the loop body block on each run.
 * The arguments given may depend on how many arguments the block takes,
 * for example looping on a map may give only keys or values to a one
 * argument block, but both to a two argument block.
 *
 * @receiver loop bounds
 * @param[7,ro restrict] $Block: loop body
 *"
public #->;
