# printer-pdf-electron-node

Uma biblioteca Node.js para impressão de arquivos PDF com suporte avançado a configurações de impressão. Funciona com Node.js e Electron.

## Características

- 🖨️ Impressão direta de PDFs para qualquer impressora do sistema
- 📏 Suporte a diferentes tamanhos de papel (A4, Letter, Legal, A3, Custom)
- ↔️ Orientação do papel (retrato ou paisagem) por trabalho de impressão
- 📑 Impressão de páginas específicas ou intervalos
- 🔄 Ajuste automático do conteúdo à página
- 📐 Configuração precisa de margens
- 🎯 Controle de DPI para qualidade de impressão
- 📚 Impressão de múltiplas cópias
- 💪 Suporte completo a TypeScript

## Requisitos

- Node.js >= 18.20.6
- Electron >= 20.0.0
- Windows ou Linux
- Para Windows: Visual Studio Build Tools
- Para Linux: CUPS development headers (`sudo apt-get install libcups2-dev`)

## Instalação

```bash
npm install printer-pdf-electron-node
```

Para desenvolvimento:
```bash
git clone https://github.com/Alexssmusica/printer-pdf-electron-node.git
cd printer-pdf-electron-node
npm install
npm run rebuild
```

## Uso

### JavaScript

```javascript
const { printPDF } = require('printer-pdf-electron-node');

// Exemplo básico
async function imprimirDocumento() {
    try {
        await printPDF({
            filePath: 'caminho/do/documento.pdf',
            printerName: 'Nome da Impressora'
        });
        console.log('Documento enviado para impressão');
    } catch (error) {
        console.error('Erro ao imprimir:', error);
    }
}

// Exemplo com todas as opções
async function imprimirDocumentoAvancado() {
    try {
        await printPDF({
            // Parâmetros obrigatórios
            filePath: 'caminho/do/documento.pdf',
            printerName: 'Nome da Impressora',
            
            // Parâmetros opcionais
            pageList: [[1, 3], [5, 7]], // Imprime páginas 1-3 e 5-7
            paperSize: 'A4',            // 'A4' | 'LETTER' | 'LEGAL' | 'A3' | 'CUSTOM'
            orientation: 'portrait',    // 'portrait' | 'landscape'
            margins: {
                top: 0.5,    // margens em polegadas
                right: 0.5,
                bottom: 0.5,
                left: 0.5
            },
            fitToPage: true,  // ajusta o conteúdo à página
            width: 0,         // largura personalizada (usado apenas com paperSize: 'CUSTOM')
            height: 0,        // altura personalizada (usado apenas com paperSize: 'CUSTOM')
            dpi: 300,        // resolução de impressão
            copies: 1        // número de cópias
        });
        console.log('Documento enviado para impressão');
    } catch (error) {
        console.error('Erro ao imprimir:', error);
    }
}
```

### TypeScript

```typescript
import { printPDF, PrintPDFOptions, PaperSize, PaperOrientation, Margins } from 'printer-pdf-electron-node';

// Definindo as margens
const margins: Margins = {
    top: 0.5,
    right: 0.5,
    bottom: 0.5,
    left: 0.5
};

// Configurando todas as opções
const options: PrintPDFOptions = {
    filePath: 'caminho/do/documento.pdf',
    printerName: 'Nome da Impressora',
    pageList: [[1, 3], [5, 7]],
    paperSize: 'A4',
    orientation: 'landscape',
    margins,
    fitToPage: true,
    dpi: 300,
    copies: 1
};

async function imprimirDocumento() {
    try {
        const resultado = await printPDF(options);
        console.log(resultado); // 'Print job created successfully'
    } catch (error) {
        console.error('Erro ao imprimir:', error);
    }
}
```

## API

### `printPDF(options: PrintPDFOptions): Promise<string>`

#### Opções

| Parâmetro    | Tipo                      | Obrigatório | Padrão  | Descrição                                    |
|--------------|---------------------------|-------------|---------|----------------------------------------------|
| filePath     | string                    | Sim         | -       | Caminho do arquivo PDF                       |
| printerName  | string                    | Sim         | -       | Nome da impressora                           |
| pageList     | Array<[number, number]>   | Não         | []      | Lista de intervalos de páginas para imprimir |
| paperSize    | PaperSize                 | Não         | 'A4'    | Tamanho do papel                            |
| orientation  | PaperOrientation          | Não         | 'portrait' | Orientação do papel (`portrait` ou `landscape`) |
| margins      | Margins                   | Não         | {0,0,0,0}| Margens em polegadas                        |
| fitToPage    | boolean                   | Não         | true    | Ajustar conteúdo à página                   |
| width        | number                    | Não         | 0       | Largura personalizada (em pontos)            |
| height       | number                    | Não         | 0       | Altura personalizada (em pontos)             |
| dpi          | number                    | Não         | 300     | Resolução de impressão                       |
| copies       | number                    | Não         | 1       | Número de cópias                             |

#### Tipos

```typescript
type PaperSize = 'A4' | 'LETTER' | 'LEGAL' | 'A3' | 'CUSTOM';
type PaperOrientation = 'portrait' | 'landscape';

interface Margins {
    top?: number;    // em polegadas
    right?: number;  // em polegadas
    bottom?: number; // em polegadas
    left?: number;   // em polegadas
}
```

## Notas

- As margens são especificadas em polegadas (1 polegada = 25.4mm)
- Para tamanhos de papel personalizados (paperSize: 'CUSTOM'), width e height são especificados em pontos (1 ponto = 1/72 polegada)
- O DPI padrão é 300, que oferece um bom equilíbrio entre qualidade e performance
- A opção fitToPage garante que o conteúdo se ajuste à área imprimível da página
- `orientation` vale para o trabalho inteiro. No Windows ela entra no DEVMODE da impressora (`DMORIENT_PORTRAIT` / `DMORIENT_LANDSCAPE`). No Linux o CUPS recebe `orientation-requested` (`3` retrato, `4` paisagem). Se algum driver CUPS ignorar esse atributo, a alternativa é a opção `landscape`

## Licença

Apache-2.0

