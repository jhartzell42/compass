"* throws none
 * prints 5
 *"
needle 2007;

public class $Test ($ConsoleProgram);

$Integer printIntTo: @output {
    @output printLine: self
}

$Test run (7,ro) {
    @out := out;
    { @@ --
	printIntTo: @out
    }(5)
}
