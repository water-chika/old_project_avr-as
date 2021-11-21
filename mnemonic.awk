#! /bin/awk -f
{
	if ($0 ~ /MNEMONIC_/) {
	sub(/,/,"",$1);
	split($1,a,"_");
	printf "    else if (0==strcmp(\"%s\",string)) mnemonic = %s;\n",tolower(a[2]),$1;
	}
	else {
		print $0;
	}
}
