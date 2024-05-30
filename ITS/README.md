# VUT FIT ITS

- **Autor:** Aurel Strigáč (xstrig00)
- **Datum:** 2024-04-04

## Matice pokrytí artefaktů

Čísla testů jednoznačně identifikují scénář v souborech `.feature`.

| Page                    | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|-------------------------|---|---|---|---|---|---|---|---|---|----|----|----|----|----|----|----|
| Home                    | x | x | x |   |   |   |   |   |   |    |    |    |    |    |    |    |
| Search                  | x | x |   |   |   |   |   |   |   |    |    |    |    |    |    |    |
| Categories              |   |   | x |   | x |   |   |   |   |    |    |    |    |    |    |    |
| Search results          | x | x | x | x |   |   |   |   |   |    |    |    |    |    |    |    |
| Product                 |   |   |   | x |   | x | x |   | x |    |    |    |    |    |    |    |
| Shopping Cart           |   |   |   |   |   | x | x | x | x | x  | x  |    |    |    |    |    |
| Checkout                |   |   |   |   |   |   |   |   |   |    | x  |    |    |    |    |    |
| Home (Admin)            |   |   |   |   |   |   |   |   |   |    |    | x  |    |    |    |    |
| Products (Admin)        |   |   |   |   |   |   |   |   |   |    |    | x  | x  | x  | x  | x  |
| Product Details (Admin) |   |   |   |   |   |   |   |   |   |    |    |    |    | x  |    | x  |


## Matice pokrytí aktivit

| Activities            | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|-----------------------|---|---|---|---|---|---|---|---|---|----|----|----|----|----|----|----|
| Searching products    | x | x | x | x | x |   |   |   |   |    |    |    |    |    |    |    |
| Buying products       |   |   |   |   |   | x | x |   | x |    |    |    |    |    |    |    |
| Editing shopping cart |   |   |   |   |   |   |   | x |   | x  |    |    |    |    |    |    |
| Checking out          |   |   |   |   |   |   |   |   |   |    | x  |    |    |    |    |    |
| Being an admin        |   |   |   |   |   |   |   |   |   |    |    | x  | x  | x  | x  | x  |
| Managing products     |   |   |   |   |   |   |   |   |   |    |    | x  | x  | x  | x  | x  |


## Matice Feature-Test

| Feature file   | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|----------------|---|---|---|---|---|---|---|---|---|----|----|----|----|----|----|----|
| search.feature | x | x | x | x | x |   |   |   |   |    |    |    |    |    |    |    |
| buy.feature    |   |   |   |   |   | x | x | x | x | x  | x  |    |    |    |    |    |
| manage.feature |   |   |   |   |   |   |   |   |   |    |    | x  | x  | x  | x  | x  |

