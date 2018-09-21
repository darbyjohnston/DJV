rm -f $1
echo '<!DOCTYPE RCC><RCC version="1.0">' >> $1
echo '<qresource>' >> $1
ls -1 *.png | awk '{print "    <file alias=\"djv/UI/"$1"\">images/"$1"</file>"}' >> $1
echo '</qresource>' >> $1
echo '</RCC>' >> $1

