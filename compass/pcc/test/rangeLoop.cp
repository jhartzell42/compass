"* throws none
 * prints 4
 * prints 5
 * prints 6
 *"
needle 2007;

"** Tests looping on a range. *"
"package test::range::loop;"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    4...6 -> { @item --
	out printLine: @item
    }
}
