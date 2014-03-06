<?php if(isset($content) && $content) :?>
<div <?php print $attributes;?> class="<?php print $classes;?>">
  <div class="block-inner">
    <?php print $content ?>
  </div>
</div>
<?php endif;?>
