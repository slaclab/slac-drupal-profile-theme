import Drupal from 'drupal';
import once from 'once';

Drupal.behaviors.arrowLink = {
  attach(context) {
    const arrowLinks = once(
      'arrow-links-prevent-wrap',
      '.c-arrow-link, .c-arrow-link--white, .c-cta-link',
      context
    );
    arrowLinks.forEach(link => {
      const text = link.textContent.trim().split(' ');
      const lastWord = text.pop();
      if (lastWord) {
        const lastWordMarkup = `<span class="c-arrow-link__word">${lastWord}</span>`;
        const lastIndex = link.innerHTML.lastIndexOf(lastWord);
        link.innerHTML =
          link.innerHTML.substring(0, lastIndex) +
          lastWordMarkup +
          link.innerHTML.substring(lastIndex + lastWord.length);
      }
    });
  },
};
