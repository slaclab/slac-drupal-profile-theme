import parse from 'html-react-parser';

import twigTemplate from './search-result.twig';
import data from './search-result.yml';

const settings = {
  title: 'Components/Search Result',
};

const SearchResult = args =>
  parse(
    twigTemplate({
      ...args,
    })
  );
SearchResult.args = { ...data };

export default settings;
export { SearchResult };
