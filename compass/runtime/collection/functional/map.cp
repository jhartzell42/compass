needle 2007;

"TODO doc"
"package compass::collection::functional::map;"

public $Collection map: @mapping {
    ^reduce: { @acc, @item -- 
	@acc adding: @mapping(@item)
    } withIdentity: species new
}
