import parse from 'html-react-parser';

import twigTemplate from './promo-box.twig';
import data from './promo-box.yml';
import { SectionWithPaddingWrapper } from '../../06-utility/storybookHelper.jsx';

const settings = {
  title: 'Paragraphs/Promo Box',
  decorators: [
    Story => (
      <SectionWithPaddingWrapper>
        <Story />
      </SectionWithPaddingWrapper>
    ),
  ],
};

const PromoBox = args =>
  parse(
    twigTemplate({
      ...args,
    })
  );
PromoBox.args = { ...data };

export default settings;
export { PromoBox };
