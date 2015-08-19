# create appendix

1. open file with vim
2. `:syntax on`
(3. `:set filetype=python`)
4. `:hardcopy > appendixA.ps`

# merge content.pdf with appendix

```
gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -sOutputFile=whitepaper.pdf content.pdf appendixA.ps appendixB.ps
```