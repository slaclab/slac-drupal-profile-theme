import parse from 'html-react-parser';

import twigTemplate from './citation.twig';
import data from './citation.yml';

const settings = {
  title: 'Components/Citation'
};

const Citation = args =>
  parse(
    twigTemplate({
      ...args,
    })
  );
Citation.args = { ...data };

export default settings;
export { Citation };
