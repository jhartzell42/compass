"* throws none
 * prints 4
 * prints 16
 * prints 9
 *"
needle 2007;

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    @out := out;
    ([2,4,3] map: { @@ -- *self}) -> { @@ --
	@out printLine: self
    }
}
