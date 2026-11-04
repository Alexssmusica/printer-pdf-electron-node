# GitHub About (busca e card do repositório)

Configure na página do repositório: clique na **engrenagem** ao lado de **About** (canto direito, abaixo do código).

## Description

Cole no campo **Description**:

```text
Print PDFs directly to system printers from Node.js and Electron. Paper size, orientation, margins, page ranges, and DPI. Windows and Linux.
```

## Website (opcional)

- `https://www.npmjs.com/package/printer-pdf-electron-node`
- ou o link do README do repositório

## Topics

Adicione estes topics (até 20):

`pdf`, `printer`, `nodejs`, `node`, `electron`, `windows`, `linux`, `typescript`, `native-addon`, `node-gyp`, `cups`, `pdfium`

## Via GitHub CLI (alternativa)

Com [GitHub CLI](https://cli.github.com/) autenticado:

```bash
gh repo edit Alexssmusica/printer-pdf-electron-node \
  --description "Print PDFs directly to system printers from Node.js and Electron. Paper size, orientation, margins, page ranges, and DPI. Windows and Linux." \
  --add-topic pdf --add-topic printer --add-topic nodejs --add-topic node \
  --add-topic electron --add-topic windows --add-topic linux --add-topic typescript \
  --add-topic native-addon --add-topic node-gyp --add-topic cups --add-topic pdfium
```

O card na busca do GitHub usa **Description**, **Topics** e trechos do **README**; a linguagem exibida segue o Linguist (ajustado via `.gitattributes` na raiz do repo).
