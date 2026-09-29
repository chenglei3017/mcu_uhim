cat oui.txt|
  sed 's/\(}\)\(.\)/\1\2\n/g'|
  sed 's/\(\",\|{\)/\1\n/g' |
  sed 's/}/\n}/g'|
  sed 's/\(^[^\[{}].*$\)/\t\1/'|
  awk '/"vid":/{next}
       {
         if($0~/mac/){
           gsub(/:\"/, ":[\n\t\t\"");
           gsub(/\"$/, "\"\n\t\t]");
           gsub(/,/, "\",\n\t\t\"");
         } print gensub(/([A-F0-9]{2})([A-F0-9]{2})([A-F0-9]{2})/, "\\1:\\2:\\3", "G");
       }' |
  sed 's/\t/  /g'
